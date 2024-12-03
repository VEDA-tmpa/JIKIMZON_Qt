#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit NetworkManager(QObject *parent = nullptr);

    void connectToVideoServer(const QString &host, quint16 port);
    void connectToJsonServer(const QString &host, quint16 port);

signals:
    void videoDataReceived(const QByteArray &videoData);
    void jsonDataReceived(const QString &jsonString);

private:
    QTcpSocket *videoSocket; // 비디오 소켓
    QTcpSocket *jsonSocket;  // JSON 소켓
    QByteArray key; // 적절한 키 값을 초기화하세요.
    QByteArray nonce; // 적절한 nonce 값을 초기화하세요.

private slots:
    void onVideoConnected();
    void onVideoReadyRead();
    void onJsonConnected();
    void onJsonReadyRead();

};

#endif // NETWORKMANAGER_H
