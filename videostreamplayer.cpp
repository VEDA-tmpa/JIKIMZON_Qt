#include "videostreamplayer.h"

void VideoStreamPlayer::InitStreamPlayer(QString ip, int port)
{
    mIp = ip;
    mPort = port;

    mServerSocket = new QTcpSocket(this);
    mServerSocket->connectToHost(mIp, mPort);
    if (!mServerSocket->waitForConnected(3000))
    {
        qDebug() << "Error: " << mServerSocket->errorString();
        return;
    }

    mbStop = true;
    mbPause = true;
}

