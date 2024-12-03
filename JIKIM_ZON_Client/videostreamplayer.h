#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include <QObject>
#include <QFile>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QTcpSocket>
#include <QPainter>

#include "decryptor.h"
#include "eventlogmanager.h"
#include "metadatadisplay.h" // MetaDataDisplay 헤더 포함

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
    explicit VideoStreamPlayer(MetaDataDisplay *metaDataDisplay = nullptr, QObject *parent = nullptr);

    ~VideoStreamPlayer();

    void processFile(const QString &filePath); //파일에서 읽어서
    void processNextFrame();
    QImage convertToQImage(AVFrame *frame);
    void processVideoDataFromMemory(const QByteArray &videoData); //실시간 스트리밍
    void decodeThread();
    void renderThread();
    //socket에서 데이터 받기
    void handleVideoData(const QByteArray &videoData);
    void handleJsonData(const QString &jsonString);
    //json파싱, 비디오 오버레이
    void parseObjectDetectionData(const QString &jsonString);
    void addOverlayToFrame(QImage &image);

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

    QTcpSocket *socket; // 소켓 멤버 변수 추가
    QByteArray key;
    QByteArray nonce;

    QVector<QRect> detectedObjects; // 객체 감지를 위한 사각형 벡터
    QStringList objectLabels; // 객체 라벨을 저장하는 리스트

    EventLogManager *eventLogManager; // EventLogManager 포인터 선언
    MetaDataDisplay *metaDataDisplay; // MetaDataDisplay 포인터
};

#endif // VIDEOSTREAMPLAYER_H
