#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include "Decryptor.h"  // Decryptor 헤더 추가
#include "eventlogmanager.h"
#include "metadatadisplay.h" // MetaDataDisplay 헤더 포함

#include <QThread>
#include <QTcpSocket>
#include <QImage>
#include <zlib.h>
#include <QList>
#include <QByteArray>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QTcpSocket>
#include <QPainter>
#include <QByteArray>
#include <QBuffer>
#include <QDataStream>
#include <QList>

#include <opencv2/opencv.hpp>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/error.h>
}

class VideoStreamPlayer : public QThread
{
    Q_OBJECT
    QByteArray key;  // key 변수 정의
public:
    explicit VideoStreamPlayer(MetaDataDisplay *metaDataDisplay = nullptr, QObject *parent = nullptr);
    VideoStreamPlayer() = default;
    ~VideoStreamPlayer();

    void startStream(QTcpSocket *socket, int frameWidth, int frameHeight, int frameSize);
    void stopStream();
    void pauseStream();
    void resumeStream();
    void storeFrame(const QImage &frame);
    void goBackward();
    void goForward();
    bool isStopped() const;
    QList<QByteArray> extractNalUnits(const QByteArray &decryptedData);
    //json파싱, 비디오 오버레이
    void parseObjectDetectionData(const QString &jsonString);
    void addOverlayToFrame(QImage &image);

signals:
    void frameReady(const QImage &frame);

protected:
    void run() override;

private:
    QTcpSocket *tcpSocket;
    bool stop;
    bool pause;  // 일시 정지 상태

    int currentFrameIndex;  // 현재 프레임 인덱스
    QList<QImage> frameHistory;  // 재생된 프레임 기록
    int frameWidth;
    int frameHeight;
    int frameSize;

    QVector<QRect> detectedObjects; // 객체 감지를 위한 사각형 벡터
    QStringList objectLabels; // 객체 라벨을 저장하는 리스트

    EventLogManager *eventLogManager; // EventLogManager 포인터 선언
    MetaDataDisplay *metaData; // MetaDataDisplay 포인터
};

#endif // VIDEOSTREAMPLAYER_H
