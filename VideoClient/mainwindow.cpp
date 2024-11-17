#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "VideoStreamWidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    tcpSocket(new QTcpSocket(this)) // tcpSocket을 MainWindow에서 생성
{
    ui->setupUi(this);

    // VideoStreamWidget에서 tcpSocket을 사용하도록 전달
    videoStreamWidget = new VideoStreamWidget(tcpSocket);  // VideoStreamWidget에서 tcpSocket 사용

    // QStackedWidget 설정 (UI에 위젯 추가)
    QStackedWidget *stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(videoStreamWidget);

    setCentralWidget(stackedWidget);  // stackedWidget을 메인 윈도우에 배치

    // 네트워크 연결 시도
    tcpSocket->connectToHost("127.0.0.1", 9999);

    // 네트워크 상태 확인 (디버그용)
    if (!tcpSocket->isValid()) {
        qDebug() << "TCP Socket is not valid!";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tcpSocket;
}
