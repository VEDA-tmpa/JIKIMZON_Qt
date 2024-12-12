#include "videostreamplayer.h"
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <vector>
#include <opencv2/opencv.hpp>
#include "frame.h"
#include <arpa/inet.h> // 엔디안 변환 함수

VideoStreamPlayer::VideoStreamPlayer(QObject *parent) : QObject(parent) {
    frame = av_frame_alloc();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoStreamPlayer::onTimerTimeout);
}

VideoStreamPlayer::~VideoStreamPlayer() {
    if (frame) av_frame_free(&frame);
    if (codecContext) avcodec_free_context(&codecContext);
    if (formatContext) avformat_close_input(&formatContext);
    if (swsContext) sws_freeContext(swsContext);
}

void VideoStreamPlayer::processFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening file!";
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    if (fileData.size() < sizeof(frame::HeaderStruct)) {
        qDebug() << "Invalid file size!";
        return;
    }

    // 헤더 역직렬화
    frame::HeaderStruct header;
    memcpy(&header, fileData.data(), sizeof(frame::HeaderStruct));

    // 엔디안 변환
    header.frameId = ntohl(header.frameId);
    header.bodySize = ntohl(header.bodySize);
    header.imageWidth = ntohs(header.imageWidth);
    header.imageHeight = ntohs(header.imageHeight);

    qDebug() << "Header information: " << header.frameId << header.bodySize
             << header.imageWidth << header.imageHeight << header.timestamp;

    // 이미지 데이터 읽기
    QByteArray imageData = fileData.mid(sizeof(frame::HeaderStruct), header.bodySize);

    // 이미지 데이터를 QImage로 변환 (RGB 형식으로 가정)
    QImage image(reinterpret_cast<const uchar*>(imageData.data()), header.imageWidth, header.imageHeight, QImage::Format_RGB888);

    // 이미지 처리 후 화면에 표시
    emit frameReady(image);

    // Optional: 이미지 형식에 맞는 처리 (JPEG, PNG 등)
    switch (static_cast<frame::ImageFormat>(header.imageFormat)) {
    case frame::ImageFormat::JPEG:
        // JPEG 처리 추가 (예: 디코딩)
        break;
    case frame::ImageFormat::PNG:
        // PNG 처리 추가 (예: 디코딩)
        break;
    case frame::ImageFormat::RAW:
        // RAW 처리 (이미 원본 데이터이므로 별도 처리 필요 없을 수 있음)
        break;
    }
}

void VideoStreamPlayer::onTimerTimeout() {
    // 타이머 타임아웃 핸들러
    // 파일에서 읽은 이미지를 일정 간격으로 처리하고자 할 때 사용
}
