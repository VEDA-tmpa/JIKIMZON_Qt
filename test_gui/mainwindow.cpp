#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "metadatadisplay.h"
#include "Frame.h"
#include <QLayout>
#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))  // 소켓 초기화
    , player(new VideoStreamPlayer(this))  // 플레이어 초기화
{
    ui->setupUi(this);

    // 초기 모드는 Light Mode로 설정
    setLightMode();

    //테마 버튼
    connect(ui->btnToggleMode, &QPushButton::clicked, this, &MainWindow::toggleMode);
    // 버튼 초기 아이콘 설정 (해 모양)
    ui->btnToggleMode->setIcon(QIcon(":/icon/sun.png"));
    ui->btnToggleMode->setIconSize(QSize(20, 20));

    // TCP 소켓 연결
    tcpSocket->connectToHost("192.168.50.14", 23456);

    // tcpSocket->connectToHost("127.0.0.1", 12345);

    // VideoStreamPlayer와 UI 연결
    connect(player, &VideoStreamPlayer::frameReady, this, [&](const QImage &frame) {
        ui->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    });

    //비디오 스트림 버튼 연결
    connect(ui->pauseButton, &QPushButton::clicked, this, [&]() {
        player->pauseStream();
    });
    connect(ui->resumeButton, &QPushButton::clicked, this, [&]() {
        player->resumeStream();
    });
    connect(ui->backwardButton, &QPushButton::clicked, player, &VideoStreamPlayer::goBackward);
    connect(ui->forwardButton, &QPushButton::clicked, player, &VideoStreamPlayer::goForward);

    // 스트림 시작
    player->startStream(tcpSocket, 1280, 720, 1280 * 720 * 3);

    // MetaDataDisplay 생성
    MetaDataDisplay* metaData = new MetaDataDisplay(this);

    // metaDataContainer에 MetaDataDisplay 추가
    if (ui->metaDataContainer->layout()) {
        ui->metaDataContainer->layout()->addWidget(metaData);
    } else {
        // 레이아웃이 없는 경우 새로 설정
        QVBoxLayout* layout = new QVBoxLayout(ui->metaDataContainer);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(metaData);
        ui->metaDataContainer->setLayout(layout);
    }

    // 메타 데이터 예시 업데이트
    metaData->updateMetaData("2024-11-15 10:20", "A구역", "paper");
}

void MainWindow::toggleMode() {
    isNightMode = !isNightMode;

    if (isNightMode) {
        setDarkMode();
        ui->btnToggleMode->setIcon(QIcon("qrc:/icon/moon.png")); // 밤 모드 아이콘
    } else {
        setLightMode();
        ui->btnToggleMode->setIcon(QIcon("qrc:/icon/sun.png")); // 낮 모드 아이콘
    }
}

void MainWindow::setLightMode() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#ffffff"));
    palette.setColor(QPalette::WindowText, QColor("#000000"));
    palette.setColor(QPalette::Base, QColor("#f5f5f5"));
    palette.setColor(QPalette::Button, QColor("#e0e0e0"));
    palette.setColor(QPalette::ButtonText, QColor("#000000"));
    qApp->setPalette(palette);
}

void MainWindow::setDarkMode() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#2b2b2b"));
    palette.setColor(QPalette::WindowText, QColor("#ffffff"));
    palette.setColor(QPalette::Base, QColor("#3b3b3b"));
    palette.setColor(QPalette::Button, QColor("#444444"));
    palette.setColor(QPalette::ButtonText, QColor("#ffffff"));
    qApp->setPalette(palette);
}

MainWindow::~MainWindow()
{
    // 리소스 정리
    player->stopStream();
    delete ui;
}

