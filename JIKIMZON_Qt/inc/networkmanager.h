#ifndef JIKIMZON_NETWORKMANAGER_H
#define JIKIMZON_NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "frame.h"

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    void ReadAllData(QAbstractSocket* socket, int expectedSize, QByteArray &buffer);
    void ReadAllData(QAbstractSocket* socket, int expectedSize, std::vector<uint8_t> &buffer);
    void connectToVideoServer(const QString &host, int port);
    void connectToJsonServer(const QString &host, int port);
    // void connectToUdpServer(const QString& host, int port);

signals:
    void videoDataReceived(QSharedPointer<frame::Header> header, QSharedPointer<std::vector<uint8_t>> videoData);
    void jsonDataReceived(QSharedPointer<QJsonDocument> jsonDoc);

private:
    QTcpSocket *mVideoSocket;
    QTcpSocket *mJsonSocket;
    QUdpSocket *mUdpSocket;

private slots:
    void onVideoConnected();
    void onVideoReadyRead();
    void onJsonConnected();
    void onJsonReadyRead();
    // void onUdpConnected();
    // void onUdpReadyRead();

};

#endif // JIKIMZON_NETWORKMANAGER_H
