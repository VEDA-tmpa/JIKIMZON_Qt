#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include <QThread>
#include <QTcpSocket>
#include <QImage>
#include <zlib.h>
#include <QByteArray>
#include <opencv2/opencv.hpp>
#include <cstdint>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

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
    void connect();
    void decoding(std::vector<uint8_t> &deserialized, cv::Mat& frame);
    void initdecoder();

signals:
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    QTcpSocket *tcpSocket;
    bool stop;
    bool pause;  // 일시 정지 상태

    SwsContext* mSwsContext;
    AVCodecContext* mCodecContext;

    int mWidth;
    int mHeight;
    int mBitrate;
    int mFps;
};

#endif // VIDEOSTREAMPLAYER_H
