#include "videostreamplayer.h"
#include "decryptor.h"
#include "deserializer.h"
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

void VideoStreamPlayer::storeFrame(const QImage &frame) {
    if (!pause) {
        frameHistory.append(frame);
        currentFrameIndex = frameHistory.size() - 1;
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

void VideoStreamPlayer::run()
{
    QByteArray buffer;

    // 키와 nonce 파일 경로
    QString keyFilePath = "/Volumes/jjeongni/QtProgramming/test/keyfile1.bin";
    QByteArray key;

    // .bin 파일에서 키 읽기
    if (!loadKey(keyFilePath, key)) {
        qDebug() << "Failed to load key from .bin file.";
        stop = true;
        return;
    }

    // Decryptor 객체 초기화
    Decryptor decryptor(key);

    while (!stop) {

        if (pause) {
            if (currentFrameIndex >= 0 && currentFrameIndex < frameHistory.size()) {
                QImage frame = frameHistory[currentFrameIndex];
                if (!frame.isNull()) {
                    emit frameReady(frame);  // 프레임 준비 상태 확인 후
                }
            }
            msleep(150);
            continue;
        }

        if (tcpSocket && tcpSocket->bytesAvailable() > 0) {
            int remainingData = frameSize - buffer.size();
            buffer.append(tcpSocket->read(remainingData));

            if (buffer.size() == frameSize) {

                QByteArray decryptedData = decryptor.decrypt(buffer);

                // // 프레임 처리
                // cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)buffer.data());
                // 복호화된 데이터를 OpenCV Mat으로 변환
                cv::Mat frame(frameHeight, frameWidth, CV_8UC3, (uchar *)decryptedData.data());
                if (!frame.empty()) {
                    qDebug() << "Frame decoded successfully.";

                    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

                    std::vector<cv::Rect> detectedObjects;
                    std::vector<std::string> labels;

                    // 예시 객체 추가
                    detectedObjects.push_back(cv::Rect(300, 100, 500, 400));
                    labels.push_back("paper");

                    // 프레임에 오버레이 추가
                    addOverlayToFrame(frame, detectedObjects, labels);

                    emit frameReady(img);

                    // 프레임 히스토리에 추가
                    frameHistory.push_back(img);
                    currentFrameIndex = frameHistory.size() - 1;  // 현재 프레임 인덱스 업데이트
                }
                buffer.clear();  // 버퍼 초기화
            }
        }
        msleep(150);  // CPU 점유율을 줄이기 위한 짧은 대기
    }
}

void VideoStreamPlayer::addOverlayToFrame(cv::Mat &frame,
                                          const std::vector<cv::Rect> &detectedObjects,
                                          const std::vector<std::string> &labels)
{
    for (size_t i = 0; i < detectedObjects.size(); i++) {
        // 라벨에 따른 색상 정의
        cv::Scalar color;
        if (labels[i] == "biodegradable") {
            color = cv::Scalar(96, 255, 0); // 초록
        } else if (labels[i] == "cardboard") {
            color = cv::Scalar(255, 0, 0); // 빨강
        } else if (labels[i] == "glass") {
            color = cv::Scalar(0, 7, 255); // 파랑
        } else if (labels[i] == "metal") {
            color = cv::Scalar(255, 148, 0); // 주황
        } else if (labels[i] == "paper") {
            color = cv::Scalar(255, 248, 0); // 노랑
        } else if (labels[i] == "plastic") {
            color = cv::Scalar(214, 0, 255); // 보라
        } else {
            color = cv::Scalar(200, 200, 200); // 기본값: 회색
        }

        // 각 객체마다 해당 색상으로 사각형 그리기 (굵기: 5)
        cv::rectangle(frame, detectedObjects[i], color, 5);

        // 라벨 텍스트 추가 (객체 이름)
        cv::putText(frame, labels[i],
                    cv::Point(detectedObjects[i].x, detectedObjects[i].y - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 2, color, 3);
    }
}

