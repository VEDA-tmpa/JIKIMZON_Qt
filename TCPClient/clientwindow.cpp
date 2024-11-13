#include "clientwindow.h"
#include "ui_clientwindow.h"
#include <QTcpSocket>
#include <vector>
#include <QHostAddress>
#include <QMessageBox>  // QMessageBox 추가
#include "serverinfo.h"

ClientWindow::ClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ClientWindow),
    socket(new QTcpSocket(this))
{
    ui->setupUi(this);

    connect(ui->connectButton, &QPushButton::clicked, this, &ClientWindow::onConnectButtonClicked);
    connect(socket, &QTcpSocket::readyRead, this, &ClientWindow::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &ClientWindow::onErrorOccurred);  // 에러 처리 연결
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::onConnectButtonClicked()
{
    if (socket->state() != QTcpSocket::ConnectedState) {
        // 서버 주소와 포트 가져오기
        QString serverAddress = ui->serverLineEdit->text();   // 서버 IP 주소
        quint16 serverPort = ui->portLineEdit->text().toUShort();  // 포트 번호

        // ServerInfo 인스턴스를 사용하여 서버 정보 설정
        ServerInfo::getInstance().setServerInfo(serverAddress, serverPort);

        // 서버에 연결 시도
        socket->connectToHost(QHostAddress(serverAddress), serverPort);

        if (!socket->waitForConnected(3000)) {
            ui->logTextEdit->append("Could not connect to server.");
            return;
        }

        ui->logTextEdit->append("Connected to server!");
    }
}

void ClientWindow::onReadyRead()
{
    QByteArray data = socket->readAll();
    std::vector<char> buffer(data.begin(), data.end());
    ui->logTextEdit->append("Received data of size: " + QString::number(buffer.size()));
}

void ClientWindow::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    QString errorMessage;
    switch (socketError) {
    case QAbstractSocket::HostNotFoundError:
        errorMessage = "The host was not found.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        errorMessage = "The connection was refused by the peer.";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        errorMessage = "The remote host is closed.";
        break;
    default:
        errorMessage = socket->errorString();
        break;
    }

    QMessageBox::critical(this, "Connection Error", errorMessage);
    ui->logTextEdit->append("Connection Error: " + errorMessage);
}
