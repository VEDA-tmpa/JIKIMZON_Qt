#include "networkmanager.h"
#include "decryptor.h"
#include <QDebug>


NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent),
    videoSocket(new QTcpSocket(this)),
    jsonSocket(new QTcpSocket(this))
{
    connect(videoSocket, &QTcpSocket::connected, this, &NetworkManager::onVideoConnected);
    connect(videoSocket, &QTcpSocket::readyRead, this, &NetworkManager::onVideoReadyRead);

    connect(jsonSocket, &QTcpSocket::connected, this, &NetworkManager::onJsonConnected);
    connect(jsonSocket, &QTcpSocket::readyRead, this, &NetworkManager::onJsonReadyRead);
}

void NetworkManager::connectToVideoServer(const QString &host, quint16 port)
{
    videoSocket->connectToHost(host, port);
}

void NetworkManager::connectToJsonServer(const QString &host, quint16 port)
{
    jsonSocket->connectToHost(host, port);
}

void NetworkManager::onVideoConnected()
{
    qDebug() << "Connected to video server";
}

// void NetworkManager::onVideoReadyRead()
// {
//     QByteArray videoData = videoSocket->readAll();

//     // 복호화 및 비디오 데이터 처리
//     QByteArray decryptedData = Decryptor::decryptChaCha20(videoData); // ChaCha20 복호화

//     // 비디오 데이터 처리 로직 추가
//     emit videoDataReceived(videoData); // 비디오 데이터 수신 시그널 발행
// }

void NetworkManager::onVideoReadyRead()
{
    QByteArray videoData = videoSocket->readAll();

    // 복호화 및 비디오 데이터 처리
    QByteArray decryptedData = Decryptor::decryptChaCha20(videoData); // ChaCha20 복호화

    // 복호화된 비디오 데이터 처리 로직 추가
    emit videoDataReceived(decryptedData); // 복호화된 비디오 데이터 수신 시그널 발행
}

void NetworkManager::onJsonConnected()
{
    qDebug() << "Connected to JSON server";
}

void NetworkManager::onJsonReadyRead()
{
    QByteArray jsonData = jsonSocket->readAll();
    QString jsonString(jsonData);

    // JSON 데이터 파싱 로직 추가
    emit jsonDataReceived(jsonString); // JSON 데이터 수신 시그널 발행
}

// void NetworkManager::onReadyRead()
// {
//     QByteArray data = socket->readAll();
//     emit dataReceived(data);
// }
