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
    wait();
}

void VideoStreamPlayer::ReadAllData(int expectedSize, OUT QByteArray& buffer)
{
    buffer.clear();
    buffer.resize(expectedSize);

    int readSize = 0;
    while (readSize < expectedSize)
    {   
        mServerSocket->waitForReadyRead(10000);
        int read = mServerSocket->read(buffer.data() + readSize, expectedSize - readSize);
        if (read == -1)
        {
            qDebug() << "Error: " << mServerSocket->errorString();
            return;
        }

        readSize += read;
    }

    qDebug() << "ReadAllData() - readSize: " << readSize;
}

void VideoStreamPlayer::InitStreamPlayer(QString ip, int port, int width, int height, int bitrate, int fps)
{   
    mWidth = width;
    mHeight = height;
    mBitrate = bitrate;
    mFps = fps;
    
    mIp = ip;
    mPort = port;

    mbStop = false;
    mbPause = false;

    // TODO: 생성 QString currentPath = QDir::currentPath(); // QCoreApplication::applicationDirPath();

    mDecodeHandler = new DecodeHandler(mHeight, mWidth, mBitrate, mFps, AVPixelFormat::AV_PIX_FMT_YUV420P, AVPixelFormat::AV_PIX_FMT_RGB24, this);
    mDecryptor = new Decryptor();
    mServerSocket = new QTcpSocket(this);
    mServerSocket->connectToHost(mIp, mPort);
    if (!mServerSocket->waitForConnected(30000))
    {
        qDebug() << "Error: " << mServerSocket->errorString();
        return;
    }

    if (mServerSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Connected to server!";
    }
    else
    {
        qDebug() << "Failed to connect to server. Current state:" << mServerSocket->state();
    }
}

void VideoStreamPlayer::run()
{
    StartStream();
}

void VideoStreamPlayer::StartStream()
{
    QByteArray headerBuffer;
    QByteArray frameBuffer;
    QByteArray decryptedFrameBuffer;
    cv::Mat cvFrame;
    // QImage로 바꿔용

    qDebug() << "started RunStreamPlayer()";

    while (!mbStop)
    {
        // if (mbPause)
        // {
        //     continue;
        // }

        if (!mServerSocket)
        {
            qDebug() << "socket not valid";
            continue;
        }

        if (!mServerSocket->waitForReadyRead(30000))
        {
            qDebug() << "Error waitForReadyRead: " << mServerSocket->errorString();
            continue;
        }

        if (mServerSocket->bytesAvailable() < sizeof(frame::HeaderStruct))
        {
            qDebug() << "error: small bytesAvailable: " << mServerSocket->bytesAvailable();
            continue;
        }

        qDebug() << "====== getting data ======";

        // get header
        headerBuffer.clear();
        ReadAllData(sizeof(frame::HeaderStruct), headerBuffer);
        if (headerBuffer.size() != sizeof(frame::HeaderStruct))
        {
            qDebug() << "header size err: " << headerBuffer.size();
            continue;
        }

        // deserialize header
        frame::Header header;
        header.Deserialize(headerBuffer);

        qDebug() << "Frame Id: " << static_cast<int>(header.GetFrameId());
        qDebug() << "Timestamp: " << QString::fromStdString(header.GetTimestamp());
        qDebug() << "Header's Body Size: " << static_cast<int>(header.GetBodySize());
        qDebug() << "Header's Body Size (no-cast): " << header.GetBodySize();

        // get body
        frameBuffer.clear();
        ReadAllData(header.GetBodySize(), frameBuffer);
        if (static_cast<int>(header.GetBodySize()) != header.GetBodySize())
        {
            qDebug() << "body size err: " << static_cast<int>(header.GetBodySize());
            continue;
        }

        // decrypt body
        QString qTimestamp = QString::fromStdString(header.GetTimestamp());
        mDecryptor->Decrypt(qTimestamp, frameBuffer, decryptedFrameBuffer);

        // deserialize body
        frame::Body body;
        body.Deserialize(decryptedFrameBuffer);

        qDebug() << "Body Size: " << body.GetImage().size();

        // decode frame and get cv::Mat
        mDecodeHandler->DecodeFrame(body.GetImage(), cvFrame);

        QImage img(cvFrame.data, cvFrame.cols, cvFrame.rows, cvFrame.step, QImage::Format_RGB888);
        emit FrameReady(img);

        // add overlay??

        // qDebug() << "Frame Decoded";
        // qDebug() << "Frame Size: " << cvFrame.size().area();
        // qDebug() << "Frame Width: " << cvFrame.cols;
        // qDebug() << "Frame Height: " << cvFrame.rows;
    }
    

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


void AddOverlayToFrame(cv::Mat &frame, const std::vector<cv::Rect> &detectedObjects, const std::vector<std::string> &labels)
{
    for (size_t i = 0; i < detectedObjects.size(); i++)
    {
        cv::Scalar color = mLabelColors[QString::fromStdString(labels[i])];
        cv::rectangle(frame, detectedObjects[i], color, 5);
        cv::putText(frame, labels[i], cv::Point(detectedObjects[i].x, detectedObjects[i].y - 10), cv::FONT_HERSHEY_SIMPLEX, 2, color, 3);
    }
}
