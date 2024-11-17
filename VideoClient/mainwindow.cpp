 #include "mainwindow.h"
#include "ui_mainwindow.h"
#include "VideoStreamWidget.h"
#include "recordingwidget.h"
#include "cameraswitchwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this)) // tcpSocket을 MainWindow에서 생성
    , centralWidget(new QWidget(this))
    , mainLayout(new QVBoxLayout(centralWidget))
{
    ui->setupUi(this);

    // 네트워크 연결 초기화
    setupNetwork();

    // UI 초기화
    initializeUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupNetwork()
{
    // 네트워크 연결 시도
    tcpSocket->connectToHost("127.0.0.1", 9999);

    // 네트워크 상태 확인 (디버그용)
    if (!tcpSocket->isValid()) {
        qDebug() << "TCP Socket is not valid!";
    }
}

void MainWindow::initializeUI()
{
    // 위젯 생성
    videoStreamWidget = new VideoStreamWidget(tcpSocket, this);
    recordingWidget = new RecordingWidget(videoStreamWidget, this);
    cameraSwitchWidget = new CameraSwitchWidget(this);

    // 비디오 스트림 크기 설정
    videoStreamWidget->setMinimumSize(640, 480); // 비디오 스트림의 최소 크기 설정

    // 레이아웃 구성
    mainLayout->addWidget(cameraSwitchWidget); // 카메라 전환 위젯 추가
    mainLayout->addWidget(videoStreamWidget);  // 비디오 스트림 위젯 추가
    mainLayout->addWidget(recordingWidget);    // 녹화 위젯 추가

    // 중앙 위젯 설정
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget); // QMainWindow에 중앙 위젯 배치
}
