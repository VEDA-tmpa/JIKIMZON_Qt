#include "videostreamplayer.h"
#include "frame.h"

#include <QDir>
#include <QString>
#include <QImage>

#include <opencv2/core.hpp>

void VideoStreamPlayer::InitStreamPlayer(QString ip, int port, int width, int height, int bitrate, int fps)
{   
    mWidth = width;
    mHeight = height;
    mBitrate = bitrate;
    mFps = fps;
    
    mIp = ip;
    mPort = port;

    mbStop = true;
    mbPause = true;

    // TODO: 디크립터 생성 QString currentPath = QDir::currentPath(); // QCoreApplication::applicationDirPath();

    mDecodeHandler = new DecodeHandler(mHeight, mWidth, mBitrate, mFps, AVPixelFormat::AV_PIX_FMT_YUV420P, AVPixelFormat::AV_PIX_FMT_RGB24, this);

    mServerSocket = new QTcpSocket(this);
    mServerSocket->connectToHost(mIp, mPort);
    if (!mServerSocket->waitForConnected(3000))
    {
        qDebug() << "Error: " << mServerSocket->errorString();
        return;
    }    
}

void VideoStreamPlayer::RunStreamPlayer()
{
    QByteArray headerBuffer;
    QByteArray frameBuffer;
    cv::Mat cvFrame;

    while (!mbStop)
    {
        if (mbPause)
        {
            continue;
        }

        if ((mServerSocket->bytesAvailable() < 0) || !mServerSocket->isOpen())
        {
            continue;
        }

        // get header
        headerBuffer.clear();
        headerBuffer = mServerSocket->read(sizeof(frame::HeaderStruct));
        if (headerBuffer.size() != sizeof(frame::HeaderStruct))
        {
            continue;
        }

        // deserialize header
        frame::Header header;
        header.Deserialize(headerBuffer);

        qDebug() << "Frame Id: " << header.GetFrameId();
        qDebug() << "Timestamp: " << header.GetTimestamp();
        qDebug() << "Body Size: " << header.GetBodySize();

        // get body
        frameBuffer.clear();
        frameBuffer = mServerSocket->read(header.GetBodySize());
        if (frameBuffer.size() != header.GetBodySize())
        {
            continue;
        }

        // deserialize body
        frame::Body body;
        body.Deserialize(frameBuffer);

        qDebug() << "Body Size: " << body.GetImage().size();
        qDebug() << "header's body size: " << header.GetBodySize();

        // decode frame and get cv::Mat
        mDecodeHandler->DecodeFrame(body.GetImage(), cvFrame);
        // QImage img(cvFrame.data, cvFrame.cols, cvFrame.rows, cvFrame.step, QImage::Format_RGB888);
        // emit frameReady(img);

        qDebug() << "Frame Decoded";
        qDebug() << "Frame Size: " << cvFrame.size();
        qDebug() << "Frame Width: " << cvFrame.cols;
        qDebug() << "Frame Height: " << cvFrame.rows;
    }
    

}



