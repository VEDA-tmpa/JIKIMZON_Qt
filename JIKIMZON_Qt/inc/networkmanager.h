#ifndef JIKIMZON_NETWORKMANAGER_H
#define JIKIMZON_NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "frame.h"

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    void ReadAllData(QTcpSocket* socket, int expectedSize, QByteArray &buffer);
    void connectToVideoServer(const QString &host, int port);
    void connectToJsonServer(const QString &host, int port);

signals:
    void videoDataReceived(QSharedPointer<frame::Header> header, QSharedPointer<QByteArray> videoData);
    void jsonDataReceived(QSharedPointer<QJsonDocument> jsonDoc);

private:
    QTcpSocket *mVideoSocket;
    QTcpSocket *mJsonSocket;

private slots:
    void onVideoConnected();
    void onVideoReadyRead();
    void onJsonConnected();
    void onJsonReadyRead();

};

#endif // JIKIMZON_NETWORKMANAGER_H