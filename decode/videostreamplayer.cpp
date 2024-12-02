#include "videostreamplayer.h"
#include "frame.h"

#include <QDir>
#include <QString>
#include <QImage>
#include <QString>
#include <string>

#include <opencv2/core.hpp>

void VideoStreamPlayer::SetVideoLabel(QLabel* label)
{
    mVideoLabel = label;
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

    // TODO: 디크립터 생성 QString currentPath = QDir::currentPath(); // QCoreApplication::applicationDirPath();

    mDecodeHandler = new DecodeHandler(mHeight, mWidth, mBitrate, mFps, AVPixelFormat::AV_PIX_FMT_YUV420P, AVPixelFormat::AV_PIX_FMT_RGB24, this);

    mServerSocket = new QTcpSocket(this);
    mServerSocket->connectToHost(mIp, mPort);
    if (!mServerSocket->waitForConnected(3000))
    {
        qDebug() << "Error: " << mServerSocket->errorString();
        return;
    }

    if (mServerSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Connected to server!";
    } else {
        qDebug() << "Failed to connect to server. Current state:" << mServerSocket->state();
    }
}

void VideoStreamPlayer::StartStream()
{
    QByteArray headerBuffer;
    QByteArray frameBuffer;
    cv::Mat cvFrame;

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

        // deserialize body
        frame::Body body;
        body.Deserialize(frameBuffer);

        qDebug() << "Body Size: " << body.GetImage().size();

        // decode frame and get cv::Mat
        mDecodeHandler->DecodeFrame(body.GetImage(), cvFrame);

        QImage img(cvFrame.data, cvFrame.cols, cvFrame.rows, cvFrame.step, QImage::Format_RGB888);
        mVideoLabel->setPixmap(QPixmap::fromImage(img));
        // emit frameReady(img);

        // qDebug() << "Frame Decoded";
        // qDebug() << "Frame Size: " << cvFrame.size().area();
        // qDebug() << "Frame Width: " << cvFrame.cols;
        // qDebug() << "Frame Height: " << cvFrame.rows;
    }
    

}



