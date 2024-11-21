#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include <QThread>
#include <QTcpSocket>
#include <QImage>
#include <opencv2/opencv.hpp>

class VideoStreamPlayer : public QThread
{
    Q_OBJECT
public:
    explicit VideoStreamPlayer(QObject *parent = nullptr);
    ~VideoStreamPlayer();

    void startStream(QTcpSocket *socket, int frameWidth, int frameHeight, int frameSize);
    void stopStream();
    bool isStopped() const;

signals:
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    QTcpSocket *tcpSocket;
    bool stop;
    int frameWidth;
    int frameHeight;
    int frameSize;
};

#endif // VIDEOSTREAMPLAYER_H
