#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

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
    bool isStopped() const;

signals:
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    QTcpSocket *tcpSocket;
    bool stop;
    bool pause;  // 일시 정지 상태
    int frameWidth;
    int frameHeight;
    int frameSize;
};

#endif // VIDEOSTREAMPLAYER_H
