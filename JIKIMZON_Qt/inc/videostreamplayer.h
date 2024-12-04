#ifndef JIKIMZON_VIDEOSTREAMPLAYER_H
#define JIKIMZON_VIDEOSTREAMPLAYER_H

#include <QObject>
#include <QLabel>
#include <QTcpSocket>
#include <QQueue>
#include <QImage>
#include <QByteArray>
#include <QVector>
#include <QMap>
#include <QSharedPointer>

#include <opencv2/core.hpp>

#include "networkmanager.h"
#include "decodehandler.h"
#include "decryptor.h"
#include "eventlogmanager.h"

class VideoStreamPlayer : public QObject
{
    Q_OBJECT

public:
    VideoStreamPlayer() = default;
    ~VideoStreamPlayer();

    void InitStreamPlayer(QString ip, int videoPort, int jsonPort, int width, int height, int bitrate, int fps);

    void StoreFrame(const QImage& frame);
    void AddOverlayToFrame(cv::Mat &frame, const std::vector<cv::Rect> &detectedObjects, const std::vector<std::string> &labels);

    // control stream
    void StartStream();
    void StopStream();
    void PauseStream();
    void ResumeStream();
    void GoBackward();
    void GoForward();

    bool IsStopped() const;

    cv::Scalar GetLabelColor(const QString& label);

signals:
    void FrameReady(const QImage& frame);
    void StreamReady();

public slots:
    void HandleVideoData(QSharedPointer<frame::Header> header, QSharedPointer<std::vector<uint8_t>> videoData);
    void HandleJsonData(QSharedPointer<QJsonDocument> jsonDoc);

private:
    NetworkManager* mNetworkManager;
    DecodeHandler* mDecodeHandler;
    Decryptor* mDecryptor;
    EventLogManager* mEventLogManager;

    QString mIp;
    int mVideoPort;
    int mJsonPort;

    int mWidth;
    int mHeight;
    int mBitrate;
    int mFps;

    int mCurFrameIndex;             // 현재 프레임 인덱스
    QVector<QImage> mFrameHistory;  // 재생된 프레임 // buffer?

    bool mbStop;
    bool mbPause;

    QVector<QRect> mDetectedObjects;
    QStringList mObjectLabels;

    QMap<QString, cv::Scalar> mLabelColors = {
        {"biodegradable", cv::Scalar(96, 255, 0)},
        {"cardboard", cv::Scalar(255, 0, 0)},
        {"glass", cv::Scalar(0, 7, 255)},
        {"metal", cv::Scalar(255, 148, 0)},
        {"paper", cv::Scalar(255, 248, 0)},
        {"plastic", cv::Scalar(214, 0, 255)},
        {"unknown", cv::Scalar(200, 200, 200)},
    };
};
#endif // JIKIMZON_VIDEOSTREAMPLAYER_H
