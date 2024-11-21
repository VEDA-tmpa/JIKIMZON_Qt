#include "videostreamplayer.h"

VideoStreamPlayer::VideoStreamPlayer(QObject *parent)
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

bool VideoStreamPlayer::isStopped() const
{
    return stop;
}

void VideoStreamPlayer::run()
{
    QByteArray buffer;
    while (!stop) {
        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            int remainingData = frameSize - buffer.size();
            buffer.append(tcpSocket->read(remainingData));

            if (buffer.size() == frameSize) {
                // 프레임 처리
                cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)buffer.data());
                if (!frame.empty()) {
                    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                    emit frameReady(img);
                }
                buffer.clear();  // 버퍼 초기화
            }
        }
        msleep(150);  // CPU 점유율을 줄이기 위한 짧은 대기
    }
}
