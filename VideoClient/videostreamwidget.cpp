#include "VideoStreamWidget.h"
#include "ui_VideoStreamWidget.h"
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QImage>
#include <QPixmap>

VideoStreamWidget::VideoStreamWidget(QTcpSocket *socket, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::VideoStreamWidget),
    tcpSocket(socket),
    timer(new QTimer(this)),
    frameReady(false)
{
    ui->setupUi(this);

    // tcpSocket을 사용하여 비디오 스트리밍 설정
    connect(tcpSocket, &QTcpSocket::readyRead, this, &VideoStreamWidget::readVideoStream);
    connect(tcpSocket, &QTcpSocket::stateChanged, this, &VideoStreamWidget::updateNetworkStatus);

    // 버튼 시그널과 슬롯 연결
    connect(ui->playButton, &QPushButton::clicked, this, &VideoStreamWidget::playVideo);
    connect(ui->pauseButton, &QPushButton::clicked, this, &VideoStreamWidget::pauseVideo);
    connect(ui->stopButton, &QPushButton::clicked, this, &VideoStreamWidget::stopVideo);

    // QTimer를 설정하여 일정 주기로 UI를 업데이트
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoStreamWidget::updateUI);

    // 100ms 또는 200ms마다 UI를 갱신
    timer->start(100);  // 100ms마다 UI를 갱신
}

VideoStreamWidget::~VideoStreamWidget()
{
    delete ui;
}

void VideoStreamWidget::readVideoStream()
{
    static const int headerSize = sizeof(quint32);

    // 헤더 데이터 읽기
    if (tcpSocket->bytesAvailable() < headerSize) {
        return;
    }

    QByteArray headerData = tcpSocket->read(headerSize);
    quint32 frameSize = *(reinterpret_cast<quint32*>(headerData.data()));

    // 프레임 데이터가 도착할 때까지 대기
    if (tcpSocket->bytesAvailable() < frameSize) {
        if (!tcpSocket->waitForReadyRead(100)) {
            return; // 데이터가 도착하지 않으면 반환
        }
    }

    // 프레임 데이터 읽기
    QByteArray frameData = tcpSocket->read(frameSize);
    std::vector<uchar> buffer(frameData.begin(), frameData.end());

    // OpenCV로 이미지 디코딩
    cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (!frame.empty()) {
        // 프레임 저장
        currentFrame = frame;
        frameReady = true;
    }
}

void VideoStreamWidget::updateUI()
{
    if (frameReady && !currentFrame.empty()) {
        // OpenCV 이미지를 QImage로 변환
        QImage img((const uchar*)currentFrame.data, currentFrame.cols, currentFrame.rows, currentFrame.step, QImage::Format_BGR888);

        // UI에서 비디오 업데이트
        ui->videoLabel->setPixmap(QPixmap::fromImage(img));
        frameReady = false;  // 다음 프레임을 준비
    }
}

void VideoStreamWidget::playVideo()
{
    // 비디오 재생
    if (!timer->isActive()) {
        timer->start(100);  // 100ms마다 UI 갱신
        qDebug() << "Video Stream Playing...";
    }
}

void VideoStreamWidget::pauseVideo()
{
    // 비디오 일시 정지
    if (timer->isActive()) {
        timer->stop();  // 타이머를 멈추어 UI 갱신을 멈춤
    }
}

void VideoStreamWidget::stopVideo()
{
    // 비디오 정지
    if (timer->isActive()) {
        timer->stop();  // 타이머를 멈추어 UI 갱신을 멈춤
        qDebug() << "Video Stream Stopped...";
    }
    currentFrame.release();  // 프레임 해제
    ui->videoLabel->clear();  // 화면 초기화
}

void VideoStreamWidget::updateNetworkStatus(QAbstractSocket::SocketState socketState)
{
    switch (socketState) {
    case QTcpSocket::ConnectedState:
        // 연결됨 상태 아이콘 및 텍스트
        ui->statusLabel->setText("Connected");
        ui->statusLabel->setStyleSheet("color: green;");
        // 녹색 아이콘 설정
        ui->statusIconLabel->setPixmap(QPixmap(":/icons/green.png").scaled(32, 32));

        // 연결된 상태에서 비디오 스트리밍 시작
        if (!timer->isActive()) {
            playVideo();  // 비디오 자동 재생 시작
        }
        break;

    case QTcpSocket::ConnectingState:
        // 연결 중 상태 아이콘 및 텍스트
        ui->statusLabel->setText("Connecting...");
        ui->statusLabel->setStyleSheet("color: orange;");
        // 노란색 아이콘 설정
        ui->statusIconLabel->setPixmap(QPixmap(":/icons/yellow.png").scaled(32, 32));

        // 연결 대기 중이면 비디오 일시 정지
        pauseVideo();  // 연결 대기 중에는 비디오 일시 정지
        break;

    case QTcpSocket::UnconnectedState:
        // 연결 끊어짐 상태 아이콘 및 텍스트
        ui->statusLabel->setText("Disconnected");
        ui->statusLabel->setStyleSheet("color: red;");
        // 빨간색 아이콘 설정
        ui->statusIconLabel->setPixmap(QPixmap(":/icons/red.png").scaled(32, 32));

        // 연결 끊어지면 비디오 스트리밍 중지
        stopVideo();  // 연결 끊어지면 비디오 스트리밍 중지
        break;

    default:
        break;
    }
}
