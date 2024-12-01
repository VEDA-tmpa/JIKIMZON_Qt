#include "videostreamplayer.h"
#include <QImage>
#include <QDebug>
#include <cstdint>  // uint8_t 사용을 위해 추가
#include <QElapsedTimer>

VideoStreamPlayer::VideoStreamPlayer(QObject *parent) : QObject(parent),
    formatContext(nullptr), codecContext(nullptr), frame(nullptr), videoStreamIndex(-1), codec(nullptr) {
    avformat_network_init();
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

        QImage image = convertToQImage(frame);
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

    // YUV → RGB 변환
    int size = width * height * 3;
    uint8_t *data = new uint8_t[size];

    // YUV에서 RGB로 변환 (YUV422, YUV420 등 처리)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int yIndex = y * frame->linesize[0] + x;
            int uIndex = (y / 2) * frame->linesize[1] + (x / 2);
            int vIndex = (y / 2) * frame->linesize[2] + (x / 2);

            uint8_t Y = frame->data[0][yIndex];
            uint8_t U = frame->data[1][uIndex];
            uint8_t V = frame->data[2][vIndex];

            int R = Y + 1.402 * (V - 128);
            int G = Y - 0.344136 * (U - 128) - 0.714136 * (V - 128);
            int B = Y + 1.772 * (U - 128);

            // 범위 제한
            R = qBound(0, R, 255);
            G = qBound(0, G, 255);
            B = qBound(0, B, 255);

            int pixelIndex = (y * width + x) * 3;
            data[pixelIndex] = R;
            data[pixelIndex + 1] = G;
            data[pixelIndex + 2] = B;
        }
    }

    // QImage로 변환
    QImage img(data, width, height, QImage::Format_RGB888);
    qDebug() << "QImage conversion completed.";

    qint64 elapsedTime = timer.elapsed();
    qDebug() << "QImage conversion took:" << elapsedTime << "ms";

    return img;
}

void VideoStreamPlayer::decodeVideoData(const QByteArray &videoData, const HeaderStruct &header) {
    // 비디오 데이터를 AVPacket으로 처리
    AVPacket packet;
    av_init_packet(&packet);

    // const uint8_t*로 캐스팅 (const 그대로 유지)
    packet.data = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(videoData.data()));
    packet.size = videoData.size();

    // 코덱 설정 및 비디오 프레임 디코딩
    if (avcodec_send_packet(codecContext, &packet) == 0) {
        qDebug() << "Packet sent to decoder.";
        if (avcodec_receive_frame(codecContext, frame) == 0) {
            QImage image = convertToQImage(frame);
            emit frameReady(image);  // 디코딩된 프레임을 화면에 표시
            qDebug() << "Frame received and emitted.";
        } else {
            qDebug() << "Failed to receive frame from decoder.";
        }
    } else {
        qDebug() << "Failed to send packet to decoder.";
    }
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

void VideoStreamPlayer::processFile(const QString &filePath) {
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

    // AVPacket packet;
    // while (av_read_frame(formatContext, &packet) >= 0) {
    //     if (packet.stream_index == videoStreamIndex) {
    //         if (avcodec_send_packet(codecContext, &packet) == 0) {
    //             processNextFrame(); // 여러 프레임 처리
    //         }
    //     }
    //     av_packet_unref(&packet);
    // }

    // QThread* decodingThread = QThread::create([this]() {
    //     AVPacket packet;
    //     while (av_read_frame(formatContext, &packet) >= 0) {
    //         if (packet.stream_index == videoStreamIndex) {
    //             if (avcodec_send_packet(codecContext, &packet) == 0) {
    //                 while (avcodec_receive_frame(codecContext, frame) == 0) {
    //                     QImage image = convertToQImage(frame);
    //                     emit frameReady(image);
    //                 }
    //             }
    //         }
    //         av_packet_unref(&packet);
    //     }
    // });
    // decodingThread->start();

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

    // 비디오 파일 데이터 열기
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

    // 코덱 열기 및 프레임 처리 (기존 processFile() 함수의 내용과 비슷)
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
    AVPacket packet;
    av_init_packet(&packet);

    // const uint8_t*로 캐스팅 (const 그대로 유지)
    packet.data = const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(videoData.data()));
    packet.size = videoData.size();

    // 코덱 설정 및 비디오 프레임 디코딩
    if (avcodec_send_packet(codecContext, &packet) == 0) {
        qDebug() << "Packet sent to decoder.";
        if (avcodec_receive_frame(codecContext, frame) == 0) {
            QImage image = convertToQImage(frame);
            emit frameReady(image);  // 디코딩된 프레임을 화면에 표시
            qDebug() << "Frame received and emitted.";
        } else {
            qDebug() << "Failed to receive frame from decoder.";
        }
    } else {
        qDebug() << "Failed to send packet to decoder.";
    }

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            if (avcodec_send_packet(codecContext, &packet) == 0) {
                processNextFrame(); // 여러 프레임 처리
            }
        }
        av_packet_unref(&packet);
    }

    // 메모리 정리
    avformat_free_context(formatContext);
    avio_context_free(&avioContext);
}

// void VideoStreamPlayer::processFile(const QString &filePath) {
//     // 파일 열기
//     if (avformat_open_input(&formatContext, filePath.toStdString().c_str(), nullptr, nullptr) < 0) {
//         qDebug() << "Failed to open video file!";
//         return;
//     }
//     qDebug() << "File opened successfully.";

//     // 스트림 정보 읽기
//     if (avformat_find_stream_info(formatContext, nullptr) < 0) {
//         qDebug() << "Failed to find stream information!";
//         return;
//     }
//     qDebug() << "Stream information found.";

//     // 비디오 스트림 찾기
//     for (int i = 0; i < formatContext->nb_streams; ++i) {
//         if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//             videoStreamIndex = i;
//             qDebug() << "Video stream found at index:" << videoStreamIndex;
//             break;
//         }
//     }

//     if (videoStreamIndex == -1) {
//         qDebug() << "No video stream found!";
//         return;
//     }

//     // 코덱 열기
//     codecContext = avcodec_alloc_context3(nullptr);
//     if (!codecContext) {
//         qDebug() << "Failed to allocate codec context!";
//         return;
//     }

//     if (avcodec_parameters_to_context(codecContext, formatContext->streams[videoStreamIndex]->codecpar) < 0) {
//         qDebug() << "Failed to copy codec parameters!";
//         return;
//     }

//     codec = avcodec_find_decoder(codecContext->codec_id);
//     if (!codec) {
//         qDebug() << "Codec not found!";
//         return;
//     }

//     if (avcodec_open2(codecContext, codec, nullptr) < 0) {
//         qDebug() << "Failed to open codec!";
//         return;
//     }

//     frame = av_frame_alloc();
//     if (!frame) {
//         qDebug() << "Failed to allocate frame!";
//         return;
//     }

//     qDebug() << "Codec and frame allocated, starting to read frames.";

//     // 비디오 프레임 디코딩
//     AVPacket packet;
//     while (av_read_frame(formatContext, &packet) >= 0) {
//         if (packet.stream_index == videoStreamIndex) {
//             if (avcodec_send_packet(codecContext, &packet) == 0) {
//                 qDebug() << "Packet sent to decoder.";
//                 processNextFrame(); // 프레임 처리
//             }
//         }
//         av_packet_unref(&packet);
//     }
// }
