#include "videostreamplayer.h"
#include <QDir>
#include <QString>

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
    QByteArray buffer;
    QByteArray headerBuffer;
    QByteArray frameBuffer;

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

        headerBuffer.clear();
        headerBuffer = mServerSocket->read(sizeof(frame::HeaderStruct));


    }
    

}



