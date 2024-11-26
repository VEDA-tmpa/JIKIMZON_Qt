#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Frame.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))  // 소켓 초기화
    , player(new VideoStreamPlayer(this))  // 플레이어 초기화
{
    ui->setupUi(this);

    // TCP 소켓 연결
    tcpSocket->connectToHost("127.0.0.1", 12345);

    // VideoStreamPlayer와 UI 연결
    connect(player, &VideoStreamPlayer::frameReady, this, [&](const QImage &frame) {
        ui->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    });

    // 일시 정지 버튼 연결
    connect(ui->pauseButton, &QPushButton::clicked, this, [&]() {
        player->pauseStream();
    });

    // 재개 버튼 연결
    connect(ui->resumeButton, &QPushButton::clicked, this, [&]() {
        player->resumeStream();
    });

    // 스트림 시작
    player->startStream(tcpSocket, 1280, 720, 1280 * 720 * 3);
}

MainWindow::~MainWindow()
{
    // 리소스 정리
    player->stopStream();
    delete ui;
}
