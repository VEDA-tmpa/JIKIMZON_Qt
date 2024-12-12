#include "widget.h"
#include "ui_widget.h"
#include <QHostAddress>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL(clicked()),
            this,              SLOT(connectButton()));

    initialize();
}

void Widget::initialize()
{
    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(readyRead()),
            this,      SLOT(readMessage()));
    connect(tcpSocket, SIGNAL(disconnected()),
            this,      SLOT(disconnected()));
}

void Widget::connectButton()
{
    QString serverip = ui->serverIP->text().trimmed();
    quint16 serverPort = ui->serverPort->text().trimmed().toUShort(); // 포트 번호를 문자열에서 변환

    // IP 주소와 포트 유효성 검사
    if (serverip.isEmpty() || serverPort == 0) {
        qDebug() << "Connection Error: Please enter a valid IP address and port number.";
        return;
    }

    QHostAddress serverAddress(serverip);

    tcpSocket->connectToHost(serverAddress, serverPort);
}


void Widget::readMessage()
{
    if(tcpSocket->bytesAvailable() >= 0)
    {
        QByteArray readData = tcpSocket->readAll();
        ui->textEdit->append(readData);
    }
}

void Widget::disconnected()
{
    qDebug() << Q_FUNC_INFO << "서버 접속 종료.";
}

Widget::~Widget()
{
    delete ui;
}

