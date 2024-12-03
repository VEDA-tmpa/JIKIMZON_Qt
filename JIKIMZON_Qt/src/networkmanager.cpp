#include "networkmanager.h"
#include "decryptor.h"
#include "frame.h"
#include "body.h"
#include "header.h"

#include <QDebug>

NetworkManager::NetworkManager(QObject *parent) :
    QObject(parent),
    mVideoSocket(new QTcpSocket(this)),
    mJsonSocket(new QTcpSocket(this))
{
    connect(mVideoSocket, &QTcpSocket::connected, this, &NetworkManager::onVideoConnected);
    connect(mVideoSocket, &QTcpSocket::readyRead, this, &NetworkManager::onVideoReadyRead);

    connect(mJsonSocket, &QTcpSocket::connected, this, &NetworkManager::onJsonConnected);
    connect(mJsonSocket, &QTcpSocket::readyRead, this, &NetworkManager::onJsonReadyRead);
}

void NetworkManager::ReadAllData(QTcpSocket* socket, int expectedSize, QByteArray& buffer)
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

    qDebug() << "ReadAllData() - readSize: " << readSize;
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

void NetworkManager::onVideoConnected()
{
    qDebug() << "Connected to video server";
}

void NetworkManager::onVideoReadyRead()
{
    QByteArray headerBuffer;
    QByteArray frameBuffer;
    frame::Header header;

    ReadAllData(mVideoSocket, sizeof(frame::HeaderStruct), headerBuffer);
    header.Deserialize(headerBuffer);

    ReadAllData(mVideoSocket, header.GetBodySize(), frameBuffer);

    emit videoDataReceived(header, frameBuffer);
}

void NetworkManager::onJsonConnected()
{
    qDebug() << "Connected to JSON server";
}

void NetworkManager::onJsonReadyRead()
{
    QByteArray jsonData = mJsonSocket->readAll();
    QString jsonString(jsonData);

    emit jsonDataReceived(jsonString);
}