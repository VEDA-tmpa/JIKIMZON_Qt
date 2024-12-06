#include "videostreamplayer.h"
#include "decryptor.h"
#include "deserializer.h"
#include <QDebug>
#include <QtCore>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QQueue>
#include <cstdint>  // uint8_t 사용을 위해 추가
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#pragma pack(push, 1)
struct HeaderStruct {
    uint32_t frameId;
    uint32_t bodySize;
    uint16_t imageWidth;
    uint16_t imageHeight;
    uint8_t imageFormat;
    uint8_t padding1[3];
    char timestamp[19];
    uint8_t padding2[1];
};
#pragma pack(pop)

// 데이터를 이진수로 변환하는 함수 (앞 20바이트만 출력)
QString toBinary(const QByteArray &data, int numBytes = 20) {
    QString result;
    int size = qMin(data.size(), numBytes); // 최대 20바이트까지만 처리

    for (int i = 0; i < size; ++i) {
        result += QString("%1 ").arg((quint8)data[i], 8, 2, QChar('0')); // 8비트로 변환
    }
    return result.trimmed();
}

VideoStreamPlayer::VideoStreamPlayer(MetaDataDisplay *metaDataDisplay, QObject *parent)
    : QThread(parent), tcpSocket(nullptr), stop(true), frameWidth(0), frameHeight(0), frameSize(0)
{

}

VideoStreamPlayer::~VideoStreamPlayer()
{
    stopStream();
    wait();  // 스레드 종료 대기
}

void VideoStreamPlayer::startStream(QTcpSocket *socket, int width, int height, int size)
{
    tcpSocket = socket;
    frameWidth = width;
    frameHeight = height;
    frameSize = size;
    stop = false;
    if (!isRunning()) {
        start(LowPriority);  // 스레드 실행
    }
}

void VideoStreamPlayer::stopStream()
{
    stop = true;
}

void VideoStreamPlayer::pauseStream()
{
    pause = true;  // 일시 정지 활성화
}

void VideoStreamPlayer::resumeStream()
{
    pause = false;  // 일시 정지 해제
}

void VideoStreamPlayer::storeFrame(const QImage &frame) {
    if (!pause) {
        frameHistory.append(frame);
        currentFrameIndex = frameHistory.size() - 1;
        if(frameHistory.size() >= 150)
        {
            frameHistory.pop_front();
        }
    }
}

void VideoStreamPlayer::goBackward() {
    if (pause && currentFrameIndex > 0) {
        --currentFrameIndex;
        QImage frame = frameHistory[currentFrameIndex];  // 직접 QImage 객체를 가져옴
        emit frameReady(frame);
        qDebug() << "Backward: Current frame index is" << currentFrameIndex;
    } else {
        qDebug() << "Backward: Cannot move. Pause state:" << pause << ", Current frame index:" << currentFrameIndex;
    }
}

void VideoStreamPlayer::goForward() {
    if (pause && currentFrameIndex < frameHistory.size() - 1) {
        ++currentFrameIndex;
        QImage frame = frameHistory[currentFrameIndex];  // 직접 QImage 객체를 가져옴
        emit frameReady(frame);
        qDebug() << "Forward: Current frame index is" << currentFrameIndex;
    } else {
        qDebug() << "Forward: Cannot move. Pause state:" << pause << ", Current frame index:" << currentFrameIndex;
    }
}

bool VideoStreamPlayer::isStopped() const
{
    return stop;
}

// void VideoStreamPlayer::run()
// {
//     QByteArray buffer;

//     // 키와 nonce 파일 경로
//     // QString keyFilePath = "/Users/kimjeonegeun/Downloads/keyfile.bin";
//     QString keyFilePath = "/Users/kimjeonegeun/Downloads/JIKIMZON_Qt-feature-gui-decode/keyfile1.bin";
//     QByteArray key;

//     // .bin 파일에서 키 읽기
//     if (!loadKey(keyFilePath, key)) {
//         qDebug() << "Failed to load key from .bin file.";
//         stop = true;
//         return;
//     }

//     // Decryptor 객체 초기화
//     Decryptor decryptor(key);

//     while (!stop) {

//         if (pause) {
//             if (currentFrameIndex >= 0 && currentFrameIndex < frameHistory.size()) {
//                 QImage frame = frameHistory[currentFrameIndex];
//                 if (!frame.isNull()) {
//                     emit frameReady(frame);  // 프레임 준비 상태 확인 후
//                 }
//             }
//             msleep(150);
//             continue;
//         }

//         if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
//             int remainingData = frameSize - buffer.size();
//             buffer.append(tcpSocket->read(remainingData));

//             if (buffer.size() == frameSize) {

//                 QByteArray decryptedData = decryptor.decrypt(buffer);

//                 // // 프레임 처리
//                 // cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)buffer.data());
//                 // 복호화된 데이터를 OpenCV Mat으로 변환
//                 cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)decryptedData.data());
//                 if (!frame.empty()) {
//                     qDebug() << "Frame decoded successfully.";

//                     cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
//                     QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

//                     std::vector<cv::Rect> detectedObjects;
//                     std::vector<std::string> labels;

//                     // 예시 객체 추가
//                     detectedObjects.push_back(cv::Rect(300, 100, 500, 400));
//                     labels.push_back("paper");

//                     // 프레임에 오버레이 추가
//                     addOverlayToFrame(frame, detectedObjects, labels);

//                     emit frameReady(img);

//                     // 프레임 히스토리에 추가
//                     frameHistory.push_back(img);
//                     currentFrameIndex = frameHistory.size() - 1;  // 현재 프레임 인덱스 업데이트
//                 }
//                 buffer.clear();  // 버퍼 초기화
//             }
//         }
//         msleep(150);  // CPU 점유율을 줄이기 위한 짧은 대기
//     }
// }


// void VideoStreamPlayer::run()
// {
//     QByteArray buffer;
//     QByteArray headerBuffer(sizeof(HeaderStruct), 0);

//     // 키와 nonce 파일 경로
//     QString keyFilePath = "/Users/kimjeonegeun/Downloads/JIKIMZON_Qt-feature-gui-decode/keyfile1.bin";
//     QByteArray key;

//     // .bin 파일에서 키 읽기
//     if (!loadKey(keyFilePath, key)) {
//         qDebug() << "Failed to load key from .bin file.";
//         stop = true;
//         return;
//     }
//     qDebug() << "Key successfully loaded. Size:" << key.size();

//     // Decryptor 객체 초기화
//     Decryptor decryptor(key);
//     qDebug() << "Decryptor initialized.";

//     // FFmpeg 초기화
//     avformat_network_init();
//     const AVCodec *codec = nullptr;
//     AVCodecContext *codecContext = nullptr;
//     AVPacket packet;
//     AVFrame *frame = av_frame_alloc();
//     struct SwsContext *swsContext = nullptr;

//     // H.264 디코더 찾기
//     codec = avcodec_find_decoder(AV_CODEC_ID_H264);
//     if (!codec) {
//         qDebug() << "H.264 codec not found!";
//         return;
//     }
//     qDebug() << "H.264 codec found.";

//     codecContext = avcodec_alloc_context3(codec);
//     if (!codecContext) {
//         qDebug() << "Failed to allocate codec context!";
//         return;
//     }
//     qDebug() << "Codec context allocated.";

//     if (avcodec_open2(codecContext, codec, nullptr) < 0) {
//         qDebug() << "Failed to open codec!";
//         return;
//     }
//     qDebug() << "Codec opened successfully.";

//     while (!stop) {
//         if (pause) {
//             if (currentFrameIndex >= 0 && currentFrameIndex < frameHistory.size()) {
//                 QImage frame = frameHistory[currentFrameIndex];
//                 if (!frame.isNull()) {
//                     emit frameReady(frame);  // 프레임 준비 상태 확인 후
//                     qDebug() << "Frame with ID:" << currentFrameIndex << "emitted.";
//                 }
//             }
//             msleep(150);
//             continue;
//         }

//         if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
//             // 헤더 데이터를 먼저 읽음
//             if (buffer.size() < sizeof(HeaderStruct)) {
//                 buffer.append(tcpSocket->read(sizeof(HeaderStruct) - buffer.size()));
//             }

//             // 헤더를 모두 읽으면 구조체로 변환
//             if (buffer.size() >= sizeof(HeaderStruct)) {
//                 memcpy(headerBuffer.data(), buffer.data(), sizeof(HeaderStruct));

//                 HeaderStruct *header = reinterpret_cast<HeaderStruct*>(headerBuffer.data());

//                 // 엔디안 변환 (네트워크 바이트 순서로 변환)
//                 header->frameId = qFromBigEndian(header->frameId);
//                 header->bodySize = qFromBigEndian(header->bodySize);
//                 header->imageWidth = qFromBigEndian(header->imageWidth);
//                 header->imageHeight = qFromBigEndian(header->imageHeight);

//                 // 헤더 정보를 사용하여 이미지 크기와 포맷 결정
//                 uint32_t bodySize = header->bodySize;
//                 uint16_t imageWidth = header->imageWidth;
//                 uint16_t imageHeight = header->imageHeight;
//                 ImageFormat format = header->imageFormat;

//                 qDebug() << "Received frame with ID:" << header->frameId;
//                 qDebug() << "Body size:" << bodySize << "Image size:" << imageWidth << "x" << imageHeight;

//                 // 본문 데이터 읽기
//                 buffer.clear();
//                 while (buffer.size() < bodySize) {
//                     buffer.append(tcpSocket->read(bodySize - buffer.size()));
//                 }
//                 qDebug() << "Received full body data of size:" << buffer.size();

//                 // 데이터 복호화
//                 QByteArray decryptedData = decryptor.decrypt(buffer);
//                 qDebug() << "Decryption successful. Data size:" << decryptedData.size();

//                 // JPEG, PNG 등의 형식 처리 (예: JPEG일 경우)
//                 if (format == ImageFormat::JPEG) {
//                     QImage img = QImage::fromData(decryptedData);
//                     if (!img.isNull()) {
//                         emit frameReady(img);
//                         frameHistory.push_back(img);
//                         currentFrameIndex = frameHistory.size() - 1;  // 현재 프레임 인덱스 업데이트
//                         qDebug() << "JPEG frame with ID:" << header->frameId << "processed.";
//                     } else {
//                         qDebug() << "Failed to load JPEG image for frame ID:" << header->frameId;
//                     }
//                 }
//                 // RAW 포맷 처리 (예: RAW 포맷의 경우 직접 OpenCV로 처리)
//                 else if (format == ImageFormat::RAW) {
//                     // RAW 포맷일 경우 OpenCV로 디코딩
//                     cv::Mat frame(imageHeight, imageWidth, CV_8UC3, (uchar*)decryptedData.data());
//                     if (!frame.empty()) {
//                         cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
//                         QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
//                         emit frameReady(img);
//                         frameHistory.push_back(img);
//                         currentFrameIndex = frameHistory.size() - 1;  // 현재 프레임 인덱스 업데이트
//                         qDebug() << "RAW frame with ID:" << header->frameId << "processed.";
//                     } else {
//                         qDebug() << "Failed to decode RAW image for frame ID:" << header->frameId;
//                     }
//                 }

//                 buffer.clear();  // 버퍼 초기화
//             }
//         }
//         msleep(200);  // CPU 점유율을 줄이기 위한 짧은 대기
//     }

//     // FFmpeg 리소스 정리
//     av_frame_free(&frame);
//     avcodec_free_context(&codecContext);
//     qDebug() << "FFmpeg resources cleaned up.";
// }


//마지막 값 똑같이 찍힌것(엣지랑)
void VideoStreamPlayer::run()
{
    QByteArray buffer;
    QByteArray headerBuffer(sizeof(HeaderStruct), 0);

    // FFmpeg 초기화
    avformat_network_init();
    SwsContext *swsContext = nullptr;

    const AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        qDebug() << "H.264 codec not found!";
        return;
    }

    AVCodecContext *codecContext = avcodec_alloc_context3(codec);
    if (!codecContext) {
        qDebug() << "Failed to allocate codec context!";
        return;
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        qDebug() << "Failed to open codec!";
        avcodec_free_context(&codecContext);
        return;
    }

    AVFrame *frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "Failed to allocate frame!";
        avcodec_free_context(&codecContext);
        return;
    }

    while (!stop) {
        if (pause) {
            if (currentFrameIndex >= 0 && currentFrameIndex < frameHistory.size()) {
                QImage frame = frameHistory[currentFrameIndex];
                if (!frame.isNull()) {
                    emit frameReady(frame);
                }
            }
            msleep(150);
            continue;
        }

        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            // 헤더 읽기
            while (buffer.size() < sizeof(HeaderStruct)) {
                buffer.append(tcpSocket->read(sizeof(HeaderStruct) - buffer.size()));
            }

            // 헤더 파싱
            memcpy(headerBuffer.data(), buffer.data(), sizeof(HeaderStruct));
            HeaderStruct *header = reinterpret_cast<HeaderStruct*>(headerBuffer.data());

            // 엔디안 문제 해결: 바디 크기, 이미지 크기 등을 BigEndian에서 LittleEndian으로 변환
            header->frameId = qFromBigEndian(header->frameId);
            header->bodySize = qFromBigEndian(header->bodySize);
            header->imageWidth = qFromBigEndian(header->imageWidth);
            header->imageHeight = qFromBigEndian(header->imageHeight);

            uint32_t bodySize = header->bodySize;
            buffer.remove(0, sizeof(HeaderStruct));  // 헤더 제거
            qDebug() << "Received frame with ID:" << header->frameId << ", Body size:" << bodySize;

            // 데이터 읽기
            while (buffer.size() < bodySize) {
                buffer.append(tcpSocket->read(bodySize - buffer.size()));
            }

            QByteArray encryptedData = buffer.left(bodySize);
            buffer.remove(0, bodySize);

            // // 데이터 복호화
            // QByteArray decryptedData = decryptor.decrypt(encryptedData);
            // if (decryptedData.isEmpty()) {
            //     qDebug() << "Decryption failed.";
            //     continue;
            // }
            // qDebug() << "Decryption successful. Data size:" << decryptedData.size();
            // qDebug() << "Decrypted data snippet (hex):" << decryptedData.mid(0, 20).toHex();

            // NAL 유닛 분리
            QByteArray startCode = QByteArray::fromHex("00000001");
            QList<QByteArray> nalUnits;
            int pos = 0;
            while ((pos = encryptedData.indexOf(startCode, pos)) != -1) {
                int nextPos = encryptedData.indexOf(startCode, pos + startCode.size());
                if (nextPos == -1) {
                    nalUnits.append(encryptedData.mid(pos));
                    break;
                } else {
                    nalUnits.append(encryptedData.mid(pos, nextPos - pos));
                }
                pos = nextPos;
            }

            qDebug() << "Number of NAL Units:" << nalUnits.size();
            for (int i = 0; i < nalUnits.size(); ++i) {
                qDebug() << "NAL Unit" << i << "Size:" << nalUnits[i].size();
            }

            // 디코딩
            for (const QByteArray &nal : nalUnits) {
                AVPacket packet;
                av_init_packet(&packet);
                packet.data = (uint8_t*)nal.data();
                packet.size = nal.size();

                int sendResult = avcodec_send_packet(codecContext, &packet);
                if (sendResult < 0) {
                    char errBuf[256];
                    av_strerror(sendResult, errBuf, sizeof(errBuf));
                    qDebug() << "Error sending packet to decoder:" << errBuf;
                    continue;
                }

                while (avcodec_receive_frame(codecContext, frame) >= 0) {
                    qDebug() << "Decoded frame PTS:" << frame->pts;

                    if (!swsContext) {
                        swsContext = sws_getContext(frame->width, frame->height, codecContext->pix_fmt,
                                                    frame->width, frame->height, AV_PIX_FMT_RGB24,
                                                    SWS_BILINEAR, nullptr, nullptr, nullptr);
                        if (!swsContext) {
                            qDebug() << "Failed to initialize SWS context!";
                            break;
                        }
                    }

                    uint8_t *rgbBuffer = new uint8_t[frame->width * frame->height * 3];
                    uint8_t *rgbData[1] = { rgbBuffer };
                    int rgbStride[1] = { frame->width * 3 };

                    int result = sws_scale(swsContext, frame->data, frame->linesize, 0, frame->height, rgbData, rgbStride);
                    if (result < 0) {
                        qDebug() << "sws_scale failed. Result:" << result;
                    } else {
                        QImage img(rgbBuffer, frame->width, frame->height, QImage::Format_RGB888);
                        if (!img.isNull()) {
                            qDebug() << "Frame successfully converted to QImage.";

                            // 오버레이 추가
                            addOverlayToFrame(img);

                            emit frameReady(img);
                            frameHistory.push_back(img);
                            currentFrameIndex = frameHistory.size() - 1;
                        } else {
                            qDebug() << "Converted QImage is null.";
                        }
                    }
                    delete[] rgbBuffer;
                }
            }
        }
        msleep(150);
    }

    // FFmpeg 리소스 해제
    if (swsContext) {
        sws_freeContext(swsContext);
    }
    av_frame_free(&frame);
    avcodec_free_context(&codecContext);
}

void VideoStreamPlayer::parseObjectDetectionData(const QString &jsonString)
{
    qDebug() << "parseObjectDetectionData";

    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject()) {
        qDebug() << "Invalid JSON data";
        return;
    }

    QJsonObject obj = doc.object();
    int frameId = obj["frameId"].toInt();
    QString timestamp = obj["timestamp"].toString();

    detectedObjects.clear();
    objectLabels.clear();

    QJsonArray objectArray = obj["object"].toArray();
    for (const QJsonValue &value : objectArray) {
        QJsonObject objData = value.toObject();
        QString className = objData["className"].toString();
        int x = objData["x"].toInt();
        int y = objData["y"].toInt();
        int width = objData["width"].toInt();
        int height = objData["height"].toInt();

        detectedObjects.append(QRect(x, y, width, height));
        objectLabels.append(className);

        // // 메타데이터 업데이트
        QString location = QString("위치: (%1, %2)").arg(x).arg(y);
        emit objectDetected(timestamp, location, className); // 메타데이터 표시 업데이트

        qDebug() << "detected object name: " << className;
    }

    // EventLogManager를 통해 이벤트 로그 저장
    if (eventLogManager) {
        eventLogManager->saveEventLog(jsonString);
    } else {
        qDebug() << "EventLogManager is not set!";
    }
}

void VideoStreamPlayer::setEventLogManager(EventLogManager *manager) {
    this->eventLogManager = manager;
}

void VideoStreamPlayer::addOverlayToFrame(QImage &image)
{
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int i = 0; i < detectedObjects.size(); i++) {
        QColor color;
        if (objectLabels[i] == "biodegradable") {
            color = QColor(96, 255, 0);
        } else if (objectLabels[i] == "cardboard") {
            color = QColor(255, 0, 0);
        } else if (objectLabels[i] == "glass") {
            color = QColor(0, 7, 255);
        } else if (objectLabels[i] == "metal") {
            color = QColor(255, 148, 0);
        } else if (objectLabels[i] == "paper") {
            color = QColor(255, 248, 0);
        } else if (objectLabels[i] == "plastic") {
            color = QColor(214, 0, 255);
        } else {
            color = QColor(200, 200, 200);
        }

        painter.setPen(QPen(color, 5));
        painter.drawRect(detectedObjects[i]);

        painter.setFont(QFont("Arial", 20));
        painter.drawText(detectedObjects[i].topLeft() - QPoint(0, 10), objectLabels[i]);
    }
}


QList<QByteArray> VideoStreamPlayer::extractNalUnits(const QByteArray &decryptedData)
{
    QList<QByteArray> nalUnits;
    QByteArray startCode = QByteArray::fromHex("00000001");
    int pos = 0;

    while ((pos = decryptedData.indexOf(startCode, pos)) != -1) {
        qDebug() << "Found NAL start code at position:" << pos;
        int nextPos = decryptedData.indexOf(startCode, pos + startCode.size());
        if (nextPos == -1) {
            nalUnits.append(decryptedData.mid(pos));
            break;
        } else {
            nalUnits.append(decryptedData.mid(pos, nextPos - pos));
        }
        pos = nextPos;
    }

    return nalUnits;
}
