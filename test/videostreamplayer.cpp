#include "videostreamplayer.h"
#include "decryptor.h"
#include <QDebug>
#include <opencv2/opencv.hpp>

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

void VideoStreamPlayer::pauseStream()
{
    pause = true;  // 일시 정지 활성화
}

void VideoStreamPlayer::resumeStream()
{
    pause = false;  // 일시 정지 해제
}

bool VideoStreamPlayer::isStopped() const
{
    return stop;
}

void VideoStreamPlayer::run()
{
    QByteArray buffer;
    Decryptor decryptor(this->key);
    while (!stop) {

        // 일시 정지 상태라면 대기
        if (pause) {
            msleep(150);  // CPU 점유율을 낮추기 위해 대기
            continue;
        }

        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            int remainingData = frameSize - buffer.size();
            buffer.append(tcpSocket->read(remainingData));

            if (buffer.size() == frameSize) {

                // 암호화된 데이터 복호화
                QByteArray encryptedData = buffer;
                QByteArray decryptedData = decryptor.decrypt(encryptedData);

                // 프레임 처리
                // cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)buffer.data());
                cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)decryptedData.data());

                if (!frame.empty()) {
                    qDebug() << "Frame decoded successfully.";

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
