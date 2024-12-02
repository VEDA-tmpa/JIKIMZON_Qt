#ifndef VIDEOSTREAMPLAYER_H
#define VIDEOSTREAMPLAYER_H

#include <QObject>
#include <QTcpSocket>
// hton ntoh 등은 qToBigEndian, qToLittleEndian 등으로 대체 가능

#include "decodehandler.h"

class VideoStreamPlayer : public QObject
{
    Q_OBJECT

public:
    VideoStreamPlayer() = default;
    ~VideoStreamPlayer() = default;

    void InitStreamPlayer(QString ip, int port, int width, int height, int bitrate, int fps);
    void RunStreamPlayer();
    void StopStream();
    void PauseStream();

private:
    DecodeHandler* mDecodeHandler;
    QTcpSocket* mServerSocket;
    QString mIp;
    int mPort;

    int mWidth;
    int mHeight;
    int mBitrate;
    int mFps;

    bool mbStop;
    bool mbPause;

signals:


};

#endif // VIDEOSTREAMPLAYER_H
