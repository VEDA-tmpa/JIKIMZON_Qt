#include "networkmanager.h"
#include "decryptor.h"
#include "frame.h"
#include "body.h"
#include "header.h"

#include <QDebug>
#include <QSharedPointer>

#include <vector>
#include <cstdint>

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent),
    mVideoSocket(new QTcpSocket(this)),
    mJsonSocket(new QTcpSocket(this))
    //mUdpSocket(new QUdpSocket(this))
{
    connect(mVideoSocket, &QTcpSocket::connected, this, &NetworkManager::onVideoConnected);
    connect(mVideoSocket, &QTcpSocket::readyRead, this, &NetworkManager::onVideoReadyRead);

    connect(mJsonSocket, &QTcpSocket::connected, this, &NetworkManager::onJsonConnected);
    connect(mJsonSocket, &QTcpSocket::readyRead, this, &NetworkManager::onJsonReadyRead);

    // connect(mUdpSocket, &QUdpSocket::readyRead, this, &NetworkManager::onUdpReadyRead);
    // connect(mUdpSocket, &QUdpSocket::connected, this, &NetworkManager::onUdpConnected);
}


void NetworkManager::ReadAllData(QAbstractSocket* socket, int expectedSize, QByteArray& buffer)
{
    buffer.clear();
    buffer.resize(expectedSize);

    int readSize = 0;
    while (readSize < expectedSize)
    {   
        socket->waitForReadyRead(10000);
        int read = socket->read(buffer.data() + readSize, expectedSize - readSize);
        if (read == -1)
        {
            qDebug() << "Error: " << socket->errorString();
            return;
        }

        readSize += read;
    }

    qDebug() << "ReadAllData(QByteArray) - readSize: " << readSize;
}


void NetworkManager::ReadAllData(QAbstractSocket* socket, int expectedSize, std::vector<uint8_t>& buffer)
{
    buffer.clear();
    buffer.resize(expectedSize);

    int readSize = 0;
    while (readSize < expectedSize)
    {
        socket->waitForReadyRead(10000);
        int read = socket->read(reinterpret_cast<char*>(buffer.data() + readSize), expectedSize - readSize);
        if (read == -1)
        {
            qDebug() << "Error: " << socket->errorString();
            return;
        }

        readSize += read;
    }

    qDebug() << "ReadAllData(vector) - readSize: " << readSize;
}

void NetworkManager::connectToVideoServer(const QString &host, int port)
{
    mVideoSocket->connectToHost(host, port);

    if (!mVideoSocket->waitForConnected(30000))
    {
        qDebug() << "Error: " << mVideoSocket->errorString();
        return;
    }

    if (mVideoSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Connected to server!";
    }
    else
    {
        qDebug() << "Failed to connect to server. Current state:" << mVideoSocket->state();
    }
}

void NetworkManager::connectToJsonServer(const QString &host, int port)
{
    mJsonSocket->connectToHost(host, port);

    if (!mJsonSocket->waitForConnected(30000))
    {
        qDebug() << "Error: " << mJsonSocket->errorString();
        return;
    }

    if (mJsonSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Connected to server!";
    }
    else
    {
        qDebug() << "Failed to connect to server. Current state:" << mJsonSocket->state();
    }
}

// void NetworkManager::connectToUdpServer(const QString& host, int port)
// {
//     mUdpSocket->connectToHost(host, port);

//     if (!mUdpSocket->waitForConnected(30000))
//     {
//         qDebug() << "Error: " << mUdpSocket->errorString();
//         return;
//     }

//     if (mUdpSocket->state() == QAbstractSocket::ConnectedState) {
//         qDebug() << "Connected to server!";
//     }
//     else
//     {
//         qDebug() << "Failed to connect to server. Current state:" << mUdpSocket->state();
//     }
// }

void NetworkManager::onVideoConnected()
{
    qDebug() << "Connected to video server";
}

void NetworkManager::onVideoReadyRead()
{
    QByteArray headerBuffer;
    // QByteArray frameBuffer;

    QSharedPointer<frame::Header> header(new frame::Header());
    //QSharedPointer<QByteArray> frameBuffer(new QByteArray());
    QSharedPointer<std::vector<uint8_t>> frameBuffer(new std::vector<uint8_t>());

    // header read
    ReadAllData(mVideoSocket, sizeof(frame::HeaderStruct), headerBuffer);
    header->Deserialize(headerBuffer);

    qDebug() << "Header: frameId: " << header->GetFrameId() << ", bodySize: " << header->GetBodySize();

    // frame body read
    ReadAllData(mVideoSocket, header->GetBodySize(), *frameBuffer);

    emit videoDataReceived(header, frameBuffer);
}

void NetworkManager::onJsonConnected()
{
    qDebug() << "Connected to JSON server";
}

void NetworkManager::onJsonReadyRead()
{
    QByteArray jsonData = mJsonSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QSharedPointer<QJsonDocument> jsonDoc(new QJsonDocument(doc));

    emit jsonDataReceived(jsonDoc);
}

// void NetworkManager::onUdpConnected()
// {
//     qDebug() << "Connected to UDP server";
// }

// void NetworkManager::onUdpReadyRead()
// {
//     QByteArray headerBuffer;

//     QSharedPointer<frame::Header> header(new frame::Header());
//     //QSharedPointer<QByteArray> frameBuffer(new QByteArray());
//     QSharedPointer<std::vector<uint8_t>> frameBuffer(new std::vector<uint8_t>());

//     ReadAllData(mUdpSocket, sizeof(frame::HeaderStruct), headerBuffer);
//     header->Deserialize(headerBuffer);

//     qDebug() << "Header: frameId: " << header->GetFrameId() << ", bodySize: " << header->GetBodySize();

//     ReadAllData(mUdpSocket, header->GetBodySize(), *frameBuffer);

//     emit videoDataReceived(header, frameBuffer);
// }
