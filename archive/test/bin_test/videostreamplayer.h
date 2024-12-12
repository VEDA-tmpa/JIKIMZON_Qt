#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QTimer>
#include <QFile>
#include <QDebug>
#include <QPixmap>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}

class VideoStreamPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoStreamPlayer(QObject *parent = nullptr);
        ~VideoStreamPlayer();

    void processFile(const QString &filePath);
    // void savePacketDataToFile(const QByteArray &packetData);


signals:
     void frameReady(const QImage &image);

private slots:
    void onTimerTimeout();

private:
    AVFormatContext *formatContext = nullptr;
    AVCodecContext *codecContext = nullptr;
    const AVCodec *codec = nullptr;  // const로 수정
    AVFrame *frame = nullptr;
    AVPacket packet;
    struct SwsContext *swsContext = nullptr;
    int videoStreamIndex = -1;
    QTimer *timer;
    uint32_t frameId = 0; // 프레임 ID를 추적하는 변수
};

#endif // VIDEOSTREAMPLAYER_H
