#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include <QObject>
#include <QFile>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QImage>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
}

enum class ImageFormat : uint8_t { RAW, JPEG, PNG };

struct HeaderStruct {
    uint32_t frameId;
    uint32_t bodySize;
    uint16_t imageWidth;
    uint16_t imageHeight;
    ImageFormat imageFormat;
    uint8_t padding1[3];
    char timestamp[19];
    uint8_t padding2[1];
};

class VideoStreamPlayer : public QObject
{
    Q_OBJECT
public:
    explicit VideoStreamPlayer(QObject *parent = nullptr);

    ~VideoStreamPlayer();

    void processFile(const QString &filePath);
    void processNextFrame();
    QImage convertToQImage(AVFrame *frame);
    // void readBinFile(const QString &filePath);
    void decodeVideoData(const QByteArray &videoData, const HeaderStruct &header);
    void processVideoDataFromMemory(const QByteArray &videoData);
    void decodeThread();
    void renderThread();

signals:
     void frameReady(const QImage &image);
     void streamReady();  // 스트림 준비 완료 시 발생할 시그널

private:
    AVFormatContext *formatContext;
    AVCodecContext *codecContext;
    int videoStreamIndex;
    const AVCodec *codec;
    AVFrame *frame;
    AVPacket packet;
    QQueue<AVFrame*> frameQueue;
    QMutex queueMutex;
    QWaitCondition waitCondition;
};

#endif // VIDEOSTREAMPLAYER_H
