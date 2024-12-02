#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include "Decryptor.h"  // Decryptor 헤더 추가
#include <QThread>
#include <QTcpSocket>
#include <QImage>
#include <zlib.h>
#include <QByteArray>
#include <opencv2/opencv.hpp>

class VideoStreamPlayer : public QThread
{
    Q_OBJECT
    QByteArray key;  // key 변수 정의
public:
    explicit VideoStreamPlayer(QObject *parent = nullptr);
    ~VideoStreamPlayer();

    void startStream(QTcpSocket *socket, int frameWidth, int frameHeight, int frameSize);
    void stopStream();
    void pauseStream();
    void resumeStream();
    void storeFrame(const QImage &frame);
    void goBackward();
    void goForward();
    bool isStopped() const;
    void addOverlayToFrame(cv::Mat &frame,
                           const std::vector<cv::Rect> &detectedObjects,
                           const std::vector<std::string> &labels);
    // void addOverlayToFrame2(cv::Mat &frame, const std::vector<cv::Rect> &detectedObjects,
    //                         const std::vector<cv::Mat> &icons);

signals:
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    QTcpSocket *tcpSocket;
    bool stop;
    bool pause;  // 일시 정지 상태

    int currentFrameIndex;  // 현재 프레임 인덱스
    QVector<QImage> frameHistory;  // 재생된 프레임 기록
    int frameWidth;
    int frameHeight;
    int frameSize;
};

#endif // VIDEOSTREAMPLAYER_H
