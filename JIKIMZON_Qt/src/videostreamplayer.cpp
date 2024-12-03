#include "videostreamplayer.h"
#include "frame.h"

#include <QDir>
#include <QString>
#include <QImage>
#include <QString>
#include <QPainter>
#include <QMap>

#include <string>

#include <opencv2/opencv.hpp>

VideoStreamPlayer::~VideoStreamPlayer()
{
    StopStream();
}

void VideoStreamPlayer::InitStreamPlayer(QString ip, int videoPort, int jsonPort, int width, int height, int bitrate, int fps)
{   
    mWidth = width;
    mHeight = height;
    mBitrate = bitrate;
    mFps = fps;
    
    mIp = ip;
    mVideoPort = videoPort;
    mJsonPort = jsonPort;

    mbStop = false;
    mbPause = false;

    QString logPath = QDir::currentPath() + "/res/event_logs.db";

    mNetworkManager = new NetworkManager();
    mDecodeHandler = new DecodeHandler(mHeight, mWidth, mBitrate, mFps, AVPixelFormat::AV_PIX_FMT_YUV420P, AVPixelFormat::AV_PIX_FMT_RGB24, this);
    mDecryptor = new Decryptor();
    mEventLogManager = new EventLogManager(logPath, this);

    mNetworkManager->connectToVideoServer(mIp, mVideoPort);
    mNetworkManager->connectToJsonServer(mIp, mJsonPort);

    connect(mNetworkManager, &NetworkManager::videoDataReceived, this, &VideoStreamPlayer::HandleVideoData);
    connect(mNetworkManager, &NetworkManager::jsonDataReceived, this, &VideoStreamPlayer::HandleJsonData);
}

void VideoStreamPlayer::HandleVideoData(frame::Header &header, QByteArray &videoData)
{
    QByteArray decryptedFrame;
    QString qTimestamp = QString::fromStdString(header.GetTimestamp());
    mDecryptor->Decrypt(qTimestamp, videoData, decryptedFrame);

    frame::Body body;
    body.Deserialize(decryptedFrame);
    
    cv::Mat cvFrame;
    mDecodeHandler->DecodeFrame(body.GetImage(), cvFrame);

    QImage img(cvFrame.data, cvFrame.cols, cvFrame.rows, cvFrame.step, QImage::Format_RGB888);

    // store frame
    StoreFrame(img);

    emit FrameReady(img);
}

void VideoStreamPlayer::HandleJsonData(const QString &jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (!doc.isObject())
    {
        qDebug() << "Invalid JSON data";
        return;
    }

    // mDetectedObjects.clear();
    // mObjectLabels.clear();

    QJsonObject obj = doc.object();

    // save event log
    mEventLogManager->saveEventLog(obj);

    int frameId = obj["frameId"].toInt();
    QString timestamp = obj["timestamp"].toString();

    QJsonArray objectArray = obj["object"].toArray();
    
    for (const QJsonValue &value : objectArray)
    {
        QJsonObject objData = value.toObject();
        QString className = objData["className"].toString();
        int x = objData["x"].toInt();
        int y = objData["y"].toInt();
        int width = objData["width"].toInt();
        int height = objData["height"].toInt();

        mDetectedObjects.append(QRect(x, y, width, height));
        mObjectLabels.append(className);

        // // 메타데이터 업데이트
        // QString location = QString("위치: (%1, %2)").arg(x).arg(y);
        // metaDataDisplay->updateMetaData(timestamp, location, className); // 메타데이터 표시 업데이트
    }
}

void VideoStreamPlayer::StartStream()
{
    
}


void VideoStreamPlayer::StopStream()
{
    mbStop = true;
}

void VideoStreamPlayer::PauseStream()
{
    mbPause = true;
}

void VideoStreamPlayer::ResumeStream()
{
    mbPause = false;
}

void VideoStreamPlayer::GoBackward()
{
    if (mCurFrameIndex > 0)
    {
        mCurFrameIndex -= 1;
        emit FrameReady(mFrameHistory[mCurFrameIndex]);

        qDebug() << "Backward: Current frame index is" << mCurFrameIndex;
    }
    else
    {
        qDebug() << "Backward: Cannot move. Pause state:" << mbPause << ", Current frame index:" << mCurFrameIndex;
    }
}

void VideoStreamPlayer::GoForward()
{
    if (mCurFrameIndex < mFrameHistory.size() - 1)
    {
        mCurFrameIndex += 1;
        emit FrameReady(mFrameHistory[mCurFrameIndex]);

        qDebug() << "Forward: Current frame index is" << mCurFrameIndex;
    }
    else
    {
        qDebug() << "Forward: Cannot move. Pause state:" << mbPause << ", Current frame index:" << mCurFrameIndex;
    }
}

bool VideoStreamPlayer::IsStopped() const
{
    return mbStop;
}

void VideoStreamPlayer::StoreFrame(const QImage &frame)
{
    if (!mbPause)
    {
        mFrameHistory.append(frame);
        mCurFrameIndex = mFrameHistory.size() - 1;
    }
}

cv::Scalar VideoStreamPlayer::GetLabelColor(const QString& label)
{
    return mLabelColors[label];
}


void VideoStreamPlayer::AddOverlayToFrame(cv::Mat &frame, const std::vector<cv::Rect> &detectedObjects, const std::vector<std::string> &labels)
{
    for (size_t i = 0; i < detectedObjects.size(); i++)
    {
        cv::Scalar color = GetLabelColor(QString::fromStdString(labels[i]));
        cv::rectangle(frame, detectedObjects[i], color, 5);
        cv::putText(frame, labels[i], cv::Point(detectedObjects[i].x, detectedObjects[i].y - 10), cv::FONT_HERSHEY_SIMPLEX, 2, color, 3);
    }
}
