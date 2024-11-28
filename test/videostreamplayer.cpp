#include "videostreamplayer.h"
#include "Decryptor.h"
#include "deserializer.h"
#include "Frame.h"
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <arpa/inet.h>

#include <QFile>
#include <QStandardPaths>


VideoStreamPlayer::VideoStreamPlayer(QObject *parent)
    : QThread(parent), tcpSocket(nullptr), stop(true)
{
    qDebug() << "VideoStreamPlayer initialized.";
}

VideoStreamPlayer::~VideoStreamPlayer()
{
    stopStream();
    wait();  // 스레드 종료 대기
    qDebug() << "VideoStreamPlayer destroyed.";
}

void VideoStreamPlayer::startStream(QTcpSocket *socket)
{
    tcpSocket = socket;
    stop = false;

    if (!isRunning()) {
        start(LowPriority);  // 스레드 실행
        qDebug() << "Thread started.";
    }
}

void VideoStreamPlayer::stopStream()
{
    stop = true;
    qDebug() << "Stream stopped.";
}

void VideoStreamPlayer::pauseStream()
{
    pause = true;  // 일시 정지 활성화
    qDebug() << "Stream paused.";
}

void VideoStreamPlayer::resumeStream()
{
    pause = false;  // 일시 정지 해제
    qDebug() << "Stream resumed.";
}

bool VideoStreamPlayer::isStopped() const
{
    return stop;
}

void VideoStreamPlayer::initdecoder()
{
    mWidth = 1280;
    mHeight = 720;
    mBitrate = 1000000;
    mFps = 15;

    // H.264 디코더 찾기
    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec)
    {
        std::cerr << "Codec not found!" << std::endl;
        exit(EXIT_FAILURE);
    }

    mCodecContext = avcodec_alloc_context3(codec);
    if (!mCodecContext)
    {
        std::cerr << "Could not allocate video codec context!" << std::endl;
        exit(EXIT_FAILURE);
    }

    mCodecContext->bit_rate = mBitrate;
    mCodecContext->width = mWidth;
    mCodecContext->height = mHeight;
    mCodecContext->time_base = AVRational{1, mFps};
    mCodecContext->framerate = AVRational{mFps, 1};
    mCodecContext->gop_size = 10;
    mCodecContext->max_b_frames = 0;
    mCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    // VideoToolbox 활성화 설정
    mCodecContext->get_format = [](AVCodecContext* s, const AVPixelFormat* fmt) {
        for (int i = 0; fmt[i] != AV_PIX_FMT_NONE; i++) {
            if (fmt[i] == AV_PIX_FMT_VIDEOTOOLBOX) {
                return AV_PIX_FMT_VIDEOTOOLBOX;
            }
        }
        return fmt[0];
    };

    // 디코더 열기
    if (avcodec_open2(mCodecContext, codec, nullptr) < 0)
    {
        std::cerr << "Cannot open codec" << std::endl;
        exit(EXIT_FAILURE);
    }

    // SWS 컨텍스트 초기화
    mSwsContext = sws_getContext(mWidth, mHeight, AV_PIX_FMT_YUV420P,
                                 mWidth, mHeight, AV_PIX_FMT_BGR24,
                                 SWS_BICUBIC, nullptr, nullptr, nullptr);

    if (!mSwsContext)
    {
        std::cerr << "Could not initialize SWS context!" << std::endl;
        exit(EXIT_FAILURE);
    }



    qDebug() << "VideoStreamPlayer::initdecoder() end";
}

// 최소 디코딩 가능한 패킷 크기 정의
const size_t MINIMUM_DECODABLE_SIZE = 1024;

// 패킷 버퍼
std::vector<uint8_t> packetBuffer;

// 패킷 누적 및 디코딩 호출
void VideoStreamPlayer::addPacketToBuffer(const std::vector<uint8_t>& deserialized) {
    packetBuffer.insert(packetBuffer.end(), deserialized.begin(), deserialized.end());

    // 디코딩 준비 상태 확인
    if (isReadyForDecoding(packetBuffer)) {
        decodeBufferedData(packetBuffer); // 디코딩 호출
        packetBuffer.clear();            // 디코딩 후 버퍼 비우기
    }
}

bool VideoStreamPlayer::isReadyForDecoding(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(frame::HeaderStruct)) {
        return false; // 헤더 크기만큼 데이터가 없으면 준비되지 않음
    }

    // 헤더에서 bodySize 확인
    frame::Header header;
    std::vector<uint8_t> headerVector(buffer.begin(), buffer.begin() + sizeof(frame::HeaderStruct));
    header.Deserialize(headerVector);

    size_t bodySize = header.GetHeaderStruct().bodySize;
    size_t totalSize = sizeof(frame::HeaderStruct) + bodySize;

    // bodySize가 너무 작거나 큰 경우 로그 출력
    if (bodySize > 1000000) {  // 예시로 1MB 이상인 경우 경고
        qWarning() << "Warning: bodySize is unusually large!" << bodySize;
    }

    return buffer.size() >= totalSize;
}

// 디코딩 수행
void VideoStreamPlayer::decodeBufferedData(const std::vector<uint8_t>& buffer) {
    // 헤더 처리
    frame::Header header;
    std::vector<uint8_t> headerVector(buffer.begin(), buffer.begin() + sizeof(frame::HeaderStruct));
    header.Deserialize(headerVector);

    // 바디 처리
    std::vector<uint8_t> bodyVector(buffer.begin() + sizeof(frame::HeaderStruct), buffer.end());
    frame::Body body;
    body.Deserialize(bodyVector);

    auto& bodyData = body.GetBodyData();
    if (bodyData.empty()) {
        qDebug() << "Body data is empty!";
        return;
    }

    qDebug() << "Body deserialized. Image size:" << bodyData.size();

    // 디코딩 및 프레임 처리
    cv::Mat frameMat;
    decoding(bodyData, frameMat);

    if (!frameMat.empty()) {
        qDebug() << "Decoding done!";
        cv::cvtColor(frameMat, frameMat, cv::COLOR_BGR2RGB);  // 색상 변환
        QImage img(frameMat.data, frameMat.cols, frameMat.rows, frameMat.step, QImage::Format_RGB888);
        emit frameReady(img);  // GUI로 프레임 전달
    } else {
        qDebug() << "Failed to decode frame.";
    }
}

void VideoStreamPlayer::decoding(const std::vector<uint8_t> &deserialized, cv::Mat& decodedFrame)
{

    AVPacket packet;
    av_init_packet(&packet);

    packet.data = const_cast<uint8_t*>(deserialized.data());
    packet.size = deserialized.size();

    qDebug() << "패킷 사이즈" <<  packet.size;

    // 패킷 전달
    int ret = avcodec_send_packet(mCodecContext, &packet);
    if (ret < 0)
    {
        std::cerr << "Error sending packet for decoding!" << std::endl;
        return;
    }

    // AVFrame 할당
    AVFrame* avFrame = av_frame_alloc();
    AVFrame* bgrFrame = av_frame_alloc();
    if (!avFrame || !bgrFrame)
    {
        std::cerr << "Could not allocate video frames!" << std::endl;
        av_frame_free(&avFrame);
        av_frame_free(&bgrFrame);
        return;
    }

    bgrFrame->format = AV_PIX_FMT_BGR24;
    bgrFrame->width = mWidth;
    bgrFrame->height = mHeight;

    // BGR 버퍼 할당
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, mWidth, mHeight, 1);
    std::vector<uint8_t> bgrBuffer(numBytes);
    av_image_fill_arrays(bgrFrame->data, bgrFrame->linesize, bgrBuffer.data(), AV_PIX_FMT_BGR24, mWidth, mHeight, 1);

    // 프레임 수신 및 변환
    ret = avcodec_receive_frame(mCodecContext, avFrame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
    {
        std::cerr << "No frame available or end of stream!" << std::endl;
    }
    else if (ret < 0)
    {
        std::cerr << "Error receiving frame from decoder!" << std::endl;
    }
    else
    {
        // YUV → BGR 변환
        sws_scale(
            mSwsContext,
            avFrame->data, avFrame->linesize,
            0, mHeight,
            bgrFrame->data, bgrFrame->linesize);

        // OpenCV Mat로 변환
        decodedFrame = cv::Mat(mHeight, mWidth, CV_8UC3, bgrBuffer.data(), bgrFrame->linesize[0]).clone();
    }

    // 메모리 해제
    av_frame_free(&avFrame);
    av_frame_free(&bgrFrame);
}

// void VideoStreamPlayer::decoding(const std::vector<uint8_t>& deserialized, cv::Mat& decodedFrame)
// {
//     AVPacket packet;
//     av_init_packet(&packet);
//     packet.data = const_cast<uint8_t*>(deserialized.data());
//     packet.size = deserialized.size();

//     if (avcodec_send_packet(mCodecContext, &packet) < 0) {
//         qDebug() << "Failed to send packet.";
//         av_packet_unref(&packet);
//         return;
//     }

//     AVFrame* avFrame = av_frame_alloc();
//     if (!avFrame) {
//         qDebug() << "Failed to allocate AVFrame.";
//         av_packet_unref(&packet);
//         return;
//     }

//     if (avcodec_receive_frame(mCodecContext, avFrame) < 0) {
//         qDebug() << "Failed to receive frame.";
//         av_frame_free(&avFrame);
//         av_packet_unref(&packet);
//         return;
//     }

//     // Convert YUV to BGR
//     AVFrame* bgrFrame = av_frame_alloc();
//     if (!bgrFrame) {
//         qDebug() << "Failed to allocate BGR frame.";
//         av_frame_free(&avFrame);
//         av_packet_unref(&packet);
//         return;
//     }

//     int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, mWidth, mHeight, 1);
//     std::vector<uint8_t> buffer(numBytes);
//     av_image_fill_arrays(bgrFrame->data, bgrFrame->linesize, buffer.data(), AV_PIX_FMT_BGR24, mWidth, mHeight, 1);

//     sws_scale(mSwsContext, avFrame->data, avFrame->linesize, 0, mHeight, bgrFrame->data, bgrFrame->linesize);

//     decodedFrame = cv::Mat(mHeight, mWidth, CV_8UC3, buffer.data()).clone();

//     // Cleanup
//     av_frame_free(&avFrame);
//     av_frame_free(&bgrFrame);
//     av_packet_unref(&packet);
// }


void VideoStreamPlayer::run()
{
    initdecoder();

    // 키 초기화 및 복호화기 생성
    QString keyFilePath = "/Volumes/jjeongni/QtProgramming/test/keyfile.bin";
    QByteArray key;
    if (!loadKey(keyFilePath, key)) {
        qDebug() << "Failed to load key from" << keyFilePath;
        stop = true;
        return;
    }

    Decryptor decryptor(key);

    qDebug() << "Stream running...";

    while (!stop) {
        // 데이터 수신
        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            qDebug() << "Receiving data from socket...";

            // 헤더 받기
            // headerBuffer 크기만큼 데이터 읽기
            QByteArray headerBuffer;
            headerBuffer = tcpSocket->read(sizeof(frame::HeaderStruct));
            qDebug() << "headerBuffer.size(): " << headerBuffer.size();
            // 데이터 확인 및 처리
            if (headerBuffer.size() != sizeof(frame::HeaderStruct)) {
                qDebug() << "Failed to read the header.";
                return;
            }

            // 헤더 역직렬화
            std::vector<uint8_t> headerVector(headerBuffer.begin(), headerBuffer.end());
            frame::Header header;
            header.Deserialize(headerVector);

            qDebug() << "Header vector size:" << headerVector.size();
            qDebug() << "Header data (hex):" << QByteArray(reinterpret_cast<const char*>(headerVector.data()), headerVector.size()).toHex();

            qDebug() << "header.GetHeaderStruct().frameId: " << header.GetHeaderStruct().frameId;
            qDebug() << "header.GetHeaderStruct().imageHeight: " << header.GetHeaderStruct().imageHeight;
            qDebug() << "header.GetHeaderStruct().imageWidth: " << header.GetHeaderStruct().imageWidth;
            qDebug() << "header.GetHeaderStruct().bodySize: " << header.GetHeaderStruct().bodySize;
            qDebug() << "header.GetHeaderStruct().timestamp: " << header.GetHeaderStruct().timestamp;

            // 정은님이 알아야 할 조건
            // tcp 를 통해 바이너리 데이터 받기
                // 1. 어? 얼마큼 받아야하지?
                    // 1-1. 아~  : header.GetHeaderStruct().bodySize 이구나
            // Body 데이터 읽기
            QByteArray bodyBuffer = tcpSocket->read(header.GetHeaderStruct().bodySize);
            if (bodyBuffer.size() != header.GetHeaderStruct().bodySize) {
                qDebug() << "Failed to read the body.";
                return;
            }

            // ㅇㅋ 그 크기만큼 받았어 ->
                // 1. 아 이거는 지기성씨가 암호화 했댓지?
                    // 우선 복호화부터 하자
            // 복호화
            // std::vector<uint8_t> decryptedBody;
            // std::vector<uint8_t> encryptedBody(bodyBuffer.begin(), bodyBuffer.end());
            // // std::vector<uint8_t> nonce(12, 0x00);
            // // std::string timestamp = header.GetHeaderStruct().timestamp;

            // // // nonce를 timestamp에서 가져오기 (마지막 12바이트 사용)
            // // if (timestamp.size() >= 12) {
            // //     std::copy(timestamp.end() - 12, timestamp.end(), nonce.begin());
            // // }


            // // QByteArray로 변환 (복호화 함수에서 QByteArray 사용 시 필요)
            // QByteArray encryptedData = QByteArray::fromRawData(
            //     reinterpret_cast<const char*>(encryptedBody.data()), encryptedBody.size());

            // // 복호화 수행
            // QByteArray decryptedData = decryptor.decrypt(encryptedData);
            // if (decryptedData.isEmpty()) {
            //     qDebug() << "Decryption failed!";
            //     continue;
            // }

            // // 복호화된 데이터를 vector<uint8_t>로 변환
            // decryptedBody.assign(decryptedData.begin(), decryptedData.end());

            // // 복호화 수행
            // QByteArray decryptedData = decryptor.decrypt(bodyBuffer);
            // if (decryptedData.isEmpty()) {
            //     qDebug() << "Decryption failed!";
            //     continue;
            // }

            // qDebug() << "Decryption successful. Decrypted body size:" << decryptedData.size();

            // // 복호화된 데이터를 역직렬화
            // std::vector<uint8_t> deserializedData(decryptedData.begin(), decryptedData.end());
            // frame::Body body;
            // body.Deserialize(deserializedData);

            // // 역직렬화된 body 데이터가 비어있지 않은지 확인
            // auto& bodyData = body.GetBodyData();
            // if (bodyData.empty()) {
            //     qDebug() << "Body data is empty!";
            //     continue;
            // }

            // qDebug() << "Body deserialized. Image size:" << bodyData.size();

            // // 이제 역직렬화된 데이터를 addPacketToBuffer에 전달
            // addPacketToBuffer(deserializedData);

            // // 바디 역직렬화
            // std::vector<uint8_t> bodyVector(decryptedData.begin(), decryptedData.end());
            // frame::Body body;
            // body.Deserialize(bodyVector);

            // auto& bodyData = body.GetBodyData();
            // if (bodyData.empty()) {
            //     qDebug() << "Body data is empty!";
            //     return;
            // }

            // qDebug() << "Body deserialized. Image size:" << bodyData.size();

            // 바디 데이터 역직렬화
            std::vector<uint8_t> bodyVector(bodyBuffer.begin(), bodyBuffer.end());
            frame::Body body;
            body.Deserialize(bodyVector);


            const auto& bodyData = body.GetBodyData();
            if (bodyData.empty()) {
                qDebug() << "Body data is empty!";
                return;
            }

            qDebug() << "Body deserialized. Image size:" << bodyData.size();

            // Example to verify start codes in body data
            for (size_t i = 0; i < bodyData.size() - 4; i++) {
                if (bodyData[i] == 0x00 && bodyData[i + 1] == 0x00 && bodyData[i + 2] == 0x01) {
                    std::cout << "Start code found at position: " << i << std::endl;
                }
            }

            qDebug() << "Body data size:" << bodyData.size();
            qDebug() << "Body data (hex):" << QByteArray(reinterpret_cast<const char*>(bodyData.data()), bodyData.size()).toHex();


            // 파일 저장 경로 설정
            QString filePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/test.h264";
            QFile file(filePath);

            if (file.open(QIODevice::WriteOnly)) {
                // // 헤더 데이터를 파일에 쓰기
                // file.write(reinterpret_cast<const char*>(headerVector.data()), headerVector.size());

                // 역직렬화된 바디 데이터를 파일에 쓰기
                file.write(reinterpret_cast<const char*>(bodyData.data()), bodyData.size());

                file.close();
                qDebug() << "Header and deserialized body data saved to:" << filePath;
            } else {
                qDebug() << "Failed to save header and deserialized body data!";
            }


            // 디코딩 및 프레임 처리
            cv::Mat frameMat;
            decoding(bodyData, frameMat);

            if (!frameMat.empty()) {
                qDebug() << "Decoding done!";
                cv::cvtColor(frameMat, frameMat, cv::COLOR_BGR2RGB);  // 색상 변환
                QImage img(frameMat.data, frameMat.cols, frameMat.rows, frameMat.step, QImage::Format_RGB888);
                emit frameReady(img);  // GUI로 프레임 전달
            }

        } else {
            qDebug() << "Waiting for data...";
        }

        msleep(150);  // CPU 사용 제한
    }

    qDebug() << "Stream has stopped.";
}

