#include "videostreamplayer.h"
#include <QImage>
#include <QDebug>
#include <cstdint>  // uint8_t 사용을 위해 추가
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

VideoStreamPlayer::VideoStreamPlayer(MetaDataDisplay *metaDataDisplay, QObject *parent) : QObject(parent),
    metaDataDisplay(metaDataDisplay),
    formatContext(nullptr),
    codecContext(nullptr),
    frame(nullptr),
    videoStreamIndex(-1),
    codec(nullptr)
{
    // eventLogManager = new EventLogManager("event_log.db", this);

    avformat_network_init();

    Decryptor::initialize(":/keyfile/keyfile.bin");
    key = Decryptor::getKey();
    nonce = Decryptor::getNonce();

    qDebug() << "VideoStreamPlayer initialized.";
}

VideoStreamPlayer::~VideoStreamPlayer() {
    if (frame) {
        av_frame_free(&frame);
        qDebug() << "Frame freed.";
    }
    if (codecContext) {
        avcodec_free_context(&codecContext);
        qDebug() << "Codec context freed.";
    }
    if (formatContext) {
        avformat_close_input(&formatContext);
        qDebug() << "Format context closed.";
    }

    // delete eventLogManager; // 메모리 해제
}

void VideoStreamPlayer::processFile(const QString &filePath) {
    qDebug() << "Opening video file at path:" << filePath; // 경로 출력

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error: Failed to open file at path:" << filePath;
        return;
    }

    qDebug() << "File successfully opened. Reading data...";

    if (avformat_open_input(&formatContext, filePath.toStdString().c_str(), nullptr, nullptr) < 0) {
        qDebug() << "Failed to open video file!";
        return;
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        qDebug() << "Failed to find stream information!";
        return;
    }

    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        qDebug() << "No video stream found!";
        return;
    }

    codecContext = avcodec_alloc_context3(nullptr);
    if (!codecContext) {
        qDebug() << "Failed to allocate codec context!";
        return;
    }

    codecContext->skip_frame = AVDISCARD_NONE;

    if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar) < 0) {
        qDebug() << "Failed to copy codec parameters!";
        return;
    }

    codec = avcodec_find_decoder(codecContext->codec_id);
    if (!codec) {
        qDebug() << "Codec not found!";
        return;
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        qDebug() << "Failed to open codec!";
        return;
    }

    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "Failed to allocate frame!";
        return;
    }

    QThread* decodingThread = QThread::create([this]() {
        AVPacket packet;
        while (av_read_frame(formatContext, &packet) >= 0) {
            if (packet.stream_index == videoStreamIndex) {
                if (avcodec_send_packet(codecContext, &packet) == 0) {
                    processNextFrame();
                }
            }
            av_packet_unref(&packet);
        }
    });
    decodingThread->start();
}


void VideoStreamPlayer::handleVideoData(const QByteArray &videoData)
{
    processVideoDataFromMemory(videoData); // 메모리에서 비디오 데이터 처리
}

void VideoStreamPlayer::handleJsonData(const QString &jsonString) {

    parseObjectDetectionData(jsonString); // JSON 데이터 파싱

    eventLogManager->saveEventLog(jsonString);
}

void VideoStreamPlayer::parseObjectDetectionData(const QString &jsonString)
{
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
        // QString location = QString("위치: (%1, %2)").arg(x).arg(y);
        // metaDataDisplay->updateMetaData(timestamp, location, className); // 메타데이터 표시 업데이트
    }
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


void VideoStreamPlayer::processNextFrame() {
    static int frameNumber = 0;  // 프레임 번호를 저장할 정적 변수

    qDebug() << "Starting to process frame number:" << frameNumber;

    static QElapsedTimer fpsTimer;
    static int frameCount = 0;
    static int64_t lastPts = -1;
    static int64_t lastTime = 0; // 마지막 타임스탬프 저장

    if (!fpsTimer.isValid()) {
        fpsTimer.start();
    }

    while (true) {
        int ret = avcodec_receive_frame(codecContext, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            qDebug() << "Error during decoding frame" << frameNumber << "!";
            break;
        }

        // 프레임의 PTS(프레젠테이션 타임스탬프) 확인
        int64_t pts = frame->pts;

        // PTS가 -1일 경우에는 시간 정보를 사용할 수 없으므로 건너뜁니다.
        if (pts == AV_NOPTS_VALUE) {
            pts = frame->best_effort_timestamp;
        }

        // 타임스탬프 차이 계산
        if (lastPts != -1) {
            int64_t timeDiff = pts - lastPts;
            qDebug() << "Time between frames:" << timeDiff << "ms";

            // 이전 프레임과 현재 프레임의 시간 차이를 기반으로 대기 시간 계산
            int64_t currentTime = fpsTimer.elapsed();  // 경과 시간 사용
            int64_t timeToWait = timeDiff - currentTime + lastTime;

            if (timeToWait > 0) {
                QThread::msleep(timeToWait);  // 적절한 시간 동안 대기
            }

            lastTime = currentTime;  // 마지막 재생 시간 갱신
        }

        lastPts = pts;  // PTS 저장

        QThread::msleep(55);

        QImage image = convertToQImage(frame);

        // 오버레이 추가
        addOverlayToFrame(image);

        emit frameReady(image);

        qDebug() << "Frame" << frameNumber << "processed successfully";
        frameNumber++;  // 프레임 번호 증가

        frameCount++;
        if (fpsTimer.elapsed() >= 1000) {
            qDebug() << "Decoder FPS:" << frameCount;
            frameCount = 0;
            fpsTimer.restart();
        }
    }
    qDebug() << "Frame processed successfully";
}

QImage VideoStreamPlayer::convertToQImage(AVFrame *frame) {
    QElapsedTimer timer;
    timer.start();

    int width = frame->width;
    int height = frame->height;
    qDebug() << "Converting frame to QImage with dimensions:" << width << "x" << height;

    // SwsContext 초기화
    SwsContext *sws_ctx = sws_getContext(width, height, static_cast<AVPixelFormat>(frame->format),
                                         width, height, AV_PIX_FMT_RGB24,
                                         SWS_BILINEAR, nullptr, nullptr, nullptr);

    // RGB 버퍼 준비
    uint8_t *rgb_buffer = new uint8_t[width * height * 3];
    uint8_t *dst_data[4] = {rgb_buffer, nullptr, nullptr, nullptr};
    int dst_linesize[4] = {width * 3, 0, 0, 0};

    // YUV에서 RGB로 변환
    sws_scale(sws_ctx, frame->data, frame->linesize, 0, height, dst_data, dst_linesize);

    // QImage 생성
    QImage img(rgb_buffer, width, height, dst_linesize[0], QImage::Format_RGB888,
               [](void *info) { delete[] static_cast<uint8_t*>(info); }, rgb_buffer);

    // 리소스 정리
    sws_freeContext(sws_ctx);

    qint64 elapsedTime = timer.elapsed();
    qDebug() << "QImage conversion completed in:" << elapsedTime << "ms";

    return img;
}

void VideoStreamPlayer::processVideoDataFromMemory(const QByteArray &videoData) {
    // AVIOContext를 통해 메모리에서 데이터를 읽도록 설정
    AVIOContext *avioContext = nullptr;
    unsigned char *data = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(videoData.data()));
    int size = videoData.size();

    avioContext = avio_alloc_context(data, size, 0, nullptr, nullptr, nullptr, nullptr);
    if (!avioContext) {
        qDebug() << "Failed to allocate AVIOContext!";
        return;
    }

    // AVFormatContext 초기화
    formatContext = avformat_alloc_context();
    if (!formatContext) {
        qDebug() << "Failed to allocate format context!";
        avio_context_free(&avioContext);
        return;
    }

    formatContext->pb = avioContext; // AVFormatContext에 AVIOContext 설정

    // 비디오 데이터 열기
    if (avformat_open_input(&formatContext, nullptr, nullptr, nullptr) < 0) {
        qDebug() << "Failed to open video data from memory!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    // 비디오 스트림 정보 읽기
    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        qDebug() << "Failed to find stream information!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    // 비디오 스트림 찾기
    for (int i = 0; i < formatContext->nb_streams; ++i) {
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            break;
        }
    }

    if (videoStreamIndex == -1) {
        qDebug() << "No video stream found!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    // 코덱 열기 및 프레임 처리
    codecContext = avcodec_alloc_context3(nullptr);
    if (!codecContext) {
        qDebug() << "Failed to allocate codec context!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar) < 0) {
        qDebug() << "Failed to copy codec parameters!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    codec = avcodec_find_decoder(codecContext->codec_id);
    if (!codec) {
        qDebug() << "Codec not found!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    if (avcodec_open2(codecContext, codec, nullptr) < 0) {
        qDebug() << "Failed to open codec!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    frame = av_frame_alloc();
    if (!frame) {
        qDebug() << "Failed to allocate frame!";
        avformat_free_context(formatContext);
        avio_context_free(&avioContext);
        return;
    }

    // 비디오 프레임 디코딩
    // AVPacket packet;

    // while (av_read_frame(formatContext, &packet) >= 0) {
    //     if (packet.stream_index == videoStreamIndex) {
    //         if (avcodec_send_packet(codecContext, &packet) == 0) {
    //             while (avcodec_receive_frame(codecContext, frame) == 0) {
    //                 QImage image = convertToQImage(frame); // QImage로 변환
    //                 emit frameReady(image); // 디코딩된 프레임을 화면에 표시
    //             }
    //         }
    //         else {
    //             qDebug() << "Failed to send packet to decoder.";
    //         }
    //         av_packet_unref(&packet); // 패킷 메모리 해제
    //     }
    // }

    QThread* decodingThread = QThread::create([this]() {
        AVPacket packet;
        while (av_read_frame(formatContext, &packet) >= 0) {
            if (packet.stream_index == videoStreamIndex) {
                if (avcodec_send_packet(codecContext, &packet) == 0) {
                    processNextFrame();
                }
            }
            av_packet_unref(&packet);
        }
    });
    decodingThread->start();

    // 메모리 정리
    av_frame_free(&frame); // 프레임 해제
    avformat_free_context(formatContext); // 포맷 컨텍스트 해제
    avio_context_free(&avioContext); // AVIO 컨텍스트 해제
}

// 디코딩 스레드
void VideoStreamPlayer::decodeThread()
{
    while (true) {
        AVPacket packet;
        if (av_read_frame(formatContext, &packet) >= 0 && packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                while (avcodec_receive_frame(codecContext, frame) == 0) {
                    QMutexLocker locker(&queueMutex);
                    frameQueue.enqueue(av_frame_clone(frame)); // 디코딩된 프레임 저장
                }
            }
        }
        av_packet_unref(&packet);
    }
}

void VideoStreamPlayer::renderThread()
{
    while (true) {
        AVFrame *frame = nullptr;

        {
            QMutexLocker locker(&queueMutex);
            if (!frameQueue.isEmpty()) {
                frame = frameQueue.dequeue();
            }
        }

        if (frame) {
            QImage image = convertToQImage(frame);
            emit frameReady(image);
            av_frame_free(&frame); // 메모리 해제
        }

        QThread::msleep(10); // UI 과부하 방지
    }
}
