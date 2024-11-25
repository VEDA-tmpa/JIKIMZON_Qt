#include "videostreamplayer.h"
#include "Decryptor.h"
#include "deserializer.h"
#include "Frame.h"
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

    // 키 초기화 및 복호화기 생성
    QString keyFilePath = "/Volumes/jjeongni/QtProgramming/test/keyfile.bin";
    QByteArray key;
    if (!loadKey(keyFilePath, key)) {
        qDebug() << "Failed to load key.";
        stop = true;
        return;
    }
    Decryptor decryptor(key);
    frame::Deserializer deserializer(key);

    while (!stop) {
        // 데이터 수신
        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            buffer.append(tcpSocket->readAll());

            // 프레임 단위로 처리
            while (buffer.size() >= frameSize) {
                QByteArray encryptedData = buffer.left(frameSize);
                buffer.remove(0, frameSize);

                // 1. 복호화
                QByteArray decryptedData = decryptor.decrypt(encryptedData);
                if (decryptedData.isEmpty()) {
                    qDebug() << "Decryption failed!";
                    continue;
                }

                // 2. 역직렬화
                std::vector<uint8_t> rawBuffer(decryptedData.begin(), decryptedData.end());
                frame::Frame frame;
                if (deserializer.DeserializeFrame(rawBuffer, frame)) {
                    qDebug() << "Frame deserialized successfully!";

                    // 3. 디코딩 및 OpenCV 처리
                    if (!frame.GetData().empty()) {
                        // std::vector<uint8_t>를 cv::Mat으로 변환
                        cv::Mat mat(frame.GetData().rows, frame.GetData().cols, CV_8UC3, frame.GetData().data);
                        cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);  // 색상 변환
                        QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
                        emit frameReady(img);
                    }
                } else {
                    qDebug() << "Failed to deserialize frame.";
                }
            }
        }
        msleep(150);  // CPU 사용 제한
    }
}
