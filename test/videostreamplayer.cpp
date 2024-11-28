#include "videostreamplayer.h"
#include "decryptor.h"
#include "deserializer.h"
#include "Frame.h"

#include <QDebug>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <arpa/inet.h>
#endif


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

void VideoStreamPlayer::startStream(QTcpSocket *socket, int width, int height, int size)
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

// void VideoStreamPlayer::decoding(std::vector<uint8_t> &deserialized, cv::Mat& frame)
// {
//     // 비디오 프레임 할당
//     AVFrame *avFrame = av_frame_alloc();
//     if (!avFrame) {
//         qDebug() << "Could not allocate video frame!";
//         avcodec_free_context(&mCodecContext);
//         return;
//     }

//     avFrame->format = mCodecContext->pix_fmt;
//     avFrame->width = mCodecContext->width;
//     avFrame->height = mCodecContext->height;
//     av_image_alloc(avFrame->data, avFrame->linesize, mCodecContext->width, mCodecContext->height, mCodecContext->pix_fmt, 32);

//     int ret = av_image_alloc(avFrame->data, avFrame->linesize, mCodecContext->width, mCodecContext->height, mCodecContext->pix_fmt, 32);
//     if (ret < 0)
//     {
//         std::cerr << "cannot allocate frame buffer" << std::endl;
//     }

//     sws_scale(mSwsContext, )

//     // H.264 비디오 패킷 준비
//     AVPacket packet;
//     av_init_packet(&packet);
//     packet.data = deserialized.data();  // bodyData는 H.264 비디오 데이터
//     packet.size = deserialized.size();

//     // 패킷을 디코딩
//     ret = avcodec_send_packet(mCodecContext, &packet);
//     if (ret < 0) {
//         qDebug() << "Error sending packet for decoding!";
//         av_frame_free(&avFrame);
//         avcodec_free_context(&mCodecContext);
//         return;
//     }

//     // 디코딩된 프레임 받기
//     ret = avcodec_receive_frame(mCodecContext, avFrame);
//     if (ret >= 0) {
//         qDebug() << "Frame decoded successfully!";

//         std::vector<uint8_t> img;
//         img.assign(packet.data, packet.data + packet.size);

//         cv::Mat mat(mHeight, mWidth, CV_8UC3, img.data());
//         mat.copyTo(frame);

//     } else {
//         qDebug() << "Error decoding frame!";
//     }
//     av_frame_free(&avFrame);
//     avcodec_free_context(&mCodecContext);
//     av_packet_unref(&packet);  // 패킷 메모리 해제
// }

void VideoStreamPlayer::decoding(std::vector<uint8_t> &deserialized, cv::Mat& frame)
{
    // AVFrame 할당
    AVFrame *avFrame = av_frame_alloc();
    if (!avFrame) {
        qDebug() << "Could not allocate video frame!";
        avcodec_free_context(&mCodecContext);
        return;
    }

    // 출력 프레임(BGR) 할당
    AVFrame *bgrFrame = av_frame_alloc();
    if (!bgrFrame) {
        qDebug() << "Could not allocate BGR frame!";
        av_frame_free(&avFrame);
        avcodec_free_context(&mCodecContext);
        return;
    }

    // BGR 프레임용 버퍼 할당
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, mWidth, mHeight, 1);
    uint8_t *bgrBuffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(bgrFrame->data, bgrFrame->linesize, bgrBuffer, AV_PIX_FMT_BGR24, mWidth, mHeight, 1);

    // 패킷 초기화
    AVPacket packet;
    av_init_packet(&packet);
    packet.data = deserialized.data();
    packet.size = deserialized.size();

    qDebug() << "Packet data size: " << packet.size;
    qDebug() << "Packet data preview: " << QByteArray::fromRawData((const char*)packet.data, 10).toHex();


    // 패킷을 디코더로 전달
    int ret = avcodec_send_packet(mCodecContext, &packet);
    if (ret < 0) {
        qDebug() << "Error sending packet for decoding!";
        av_frame_free(&avFrame);
        av_free(bgrBuffer);
        av_frame_free(&bgrFrame);
        return;
    }

    // 디코더에서 프레임 가져오기
    ret = avcodec_receive_frame(mCodecContext, avFrame);
    if (ret >= 0) {
        qDebug() << "Frame decoded successfully!";

        // YUV → BGR 변환
        sws_scale(
            mSwsContext,
            avFrame->data, avFrame->linesize,  // 입력 프레임 데이터
            0, mHeight,                       // 입력 프레임 범위
            bgrFrame->data, bgrFrame->linesize // 출력 프레임 데이터
            );

        // OpenCV Mat로 변환
        frame = cv::Mat(mHeight, mWidth, CV_8UC3, bgrFrame->data[0]).clone();
    } else {
        qDebug() << "Error decoding frame!";
    }

    // 메모리 해제
    av_frame_free(&avFrame);
    av_frame_free(&bgrFrame);
    av_free(bgrBuffer);
    av_packet_unref(&packet);
}


void VideoStreamPlayer::initdecoder()
{
    mWidth = 1280;
    mHeight = 720;
    mBitrate = 1000000;
    mFps = 15;

    // H.264 디코더 찾기
    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "Codec not found!";
        return;
    }

    // 디코더 컨텍스트 생성
    mCodecContext = avcodec_alloc_context3(codec);
    if (! mCodecContext) {
        qDebug() << "Could not allocate video codec context!";
        return;
    }

    mCodecContext->bit_rate = mBitrate;
    mCodecContext->width = mWidth;
    mCodecContext->height = mHeight;
    mCodecContext->time_base = (AVRational){1, mFps};
    mCodecContext->framerate = (AVRational){mFps, 1};
    mCodecContext->gop_size = 10;
    mCodecContext->max_b_frames = 0;
    mCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;

    AVDictionary *param = nullptr;
    av_dict_set(&param, "preset", "ultrafast", 0);
    av_dict_set(&param, "tune", "zerolatency", 0);

    if (avcodec_open2(mCodecContext, codec, &param) < 0)
    {
        perror("Cannot open codec");
    }

    mSwsContext = sws_getContext(mWidth, mHeight,AV_PIX_FMT_YUV420P, mWidth, mHeight,  AV_PIX_FMT_BGR24, SWS_BICUBIC, nullptr, nullptr, nullptr);
}


void VideoStreamPlayer::run()
{
    QByteArray buffer;

    // 키 초기화 및 복호화기 생성
    QString keyFilePath = "/home/sihyeon/workspace/JIKIMZON_Qt/test/keyfile.bin";
    QByteArray key;
    if (!loadKey(keyFilePath, key)) {
        qDebug() << "Failed to load key from" << keyFilePath;
        stop = true;
        return;
    }
    Decryptor decryptor(key);
    frame::Deserializer deserializer(key);

    qDebug() << "Stream running...";

    initdecoder();

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


            qDebug() << "header.GetHeaderStruct().frameId: " << header.GetHeaderStruct().frameId;
            qDebug() << "header.GetHeaderStruct().imageHeight: " << header.GetHeaderStruct().imageHeight;
            qDebug() << "header.GetHeaderStruct().imageWidth: " << header.GetHeaderStruct().imageWidth;
            qDebug() << "header.GetHeaderStruct().bodySize: " << header.GetHeaderStruct().bodySize;
            qDebug() << "header.GetHeaderStruct().timestamp: " << header.GetHeaderStruct().timestamp;

            // 정은님이 알아야 할 조건
            // tcp 를 통해 바이너리 데이터 받기
                // 1. 어? 얼마큼 받아야하지?
                    // 1-1. 아~  : header.GetHeaderStruct().bodySize 이구나
            QByteArray bodyBuffer;
            bodyBuffer = tcpSocket->read(header.GetHeaderStruct().bodySize);
            qDebug() << "header.GetHeaderStruct().bodySize: " << header.GetHeaderStruct().bodySize;
            // 데이터 확인 및 처리
            if (bodyBuffer.size() != header.GetHeaderStruct().bodySize) {
                qDebug() << "Failed to read the body.";
                return;
            }

            // ㅇㅋ 그 크기만큼 받았어 ->
                // 1. 아 이거는 지기성씨가 암호화 했댓지?
                    // 우선 복호화부터 하자
            // 복호화
            std::vector<uint8_t> decryptedBody;
            std::vector<uint8_t> encryptedBody(bodyBuffer.begin(), bodyBuffer.end());
            std::vector<uint8_t> nonce(12, 0x00);
            std::string timestamp = header.GetHeaderStruct().timestamp;

            // nonce를 timestamp에서 가져오기 (마지막 12바이트 사용)
            if (timestamp.size() >= 12) {
                std::copy(timestamp.end() - 12, timestamp.end(), nonce.begin());
            }

            // QByteArray로 변환 (복호화 함수에서 QByteArray 사용 시 필요)
            QByteArray encryptedData = QByteArray::fromRawData(
                reinterpret_cast<const char*>(encryptedBody.data()), encryptedBody.size());

            // 복호화 수행
            QByteArray decryptedData = decryptor.decrypt(encryptedData);
            if (decryptedData.isEmpty()) {
                qDebug() << "Decryption failed!";
                continue;
            }

            // 복호화된 데이터를 vector<uint8_t>로 변환
            decryptedBody.assign(decryptedData.begin(), decryptedData.end());

            qDebug() << "Decryption successful. Decrypted body size:" << decryptedBody.size();


            // 2. (역직렬화) : body 구조 : 어? image밖에 없네
            std::vector<uint8_t> bodyVector(decryptedBody.begin(), decryptedBody.end());
            frame::Body body;
            body.Deserialize(bodyVector);

            // 2. Body 데이터 확인
            auto& bodyData = body.GetBodyData();
            if (bodyData.empty()) {
                qDebug() << "Body data is empty!";
                return;
            }         

            // Body 데이터 확인
            qDebug() << "Body deserialized. Image size:" << body.GetBodyData().size();

            cv::Mat frameMat;
            decoding(bodyData, frameMat);

            if (!frameMat.empty()) {
                cv::cvtColor(frameMat, frameMat, cv::COLOR_BGR2RGB);  // 색상 변환
                QImage img(frameMat.data, frameMat.cols, frameMat.rows, frameMat.step, QImage::Format_RGB888);
                emit frameReady(img);  // 이 데이터를 GUI로 전달
            }

        } else {
            qDebug() << "Waiting for data...";
        }

        msleep(150);  // CPU 사용 제한
    }

    qDebug() << "Stream has stopped.";
}
