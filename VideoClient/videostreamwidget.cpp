#include "VideoStreamWidget.h"
#include "ui_VideoStreamWidget.h"
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <cstdint> // uint32_t 사용
#include <QElapsedTimer>
#include <QDateTime>
#include <QStandardPaths>
#include "framespecs.h"
#include <QThread>


VideoStreamWidget::VideoStreamWidget(QTcpSocket *socket, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::VideoStreamWidget),
    tcpSocket(socket),
    timer(new QTimer(this)),
    frameReady(false),
    isFullScreen(false) // 초기값 설정
{
    ui->setupUi(this);

    // tcpSocket을 사용하여 비디오 스트리밍 설정
    connect(tcpSocket, &QTcpSocket::readyRead, this, &VideoStreamWidget::receiveFrame);
    connect(tcpSocket, &QTcpSocket::stateChanged, this, &VideoStreamWidget::updateNetworkStatus);

    // 버튼 시그널과 슬롯 연결
    connect(ui->playButton, &QPushButton::clicked, this, &VideoStreamWidget::playVideo);
    connect(ui->pauseButton, &QPushButton::clicked, this, &VideoStreamWidget::pauseVideo);
    connect(ui->stopButton, &QPushButton::clicked, this, &VideoStreamWidget::stopVideo);

    // 전체 화면 버튼 시그널 연결
    connect(ui->fullScreenButton, &QPushButton::clicked, this, &VideoStreamWidget::toggleFullScreen);
}

VideoStreamWidget::~VideoStreamWidget()
{
    if (fullScreenWindow) {
        delete fullScreenWindow;
    }
    delete ui;
}

void VideoStreamWidget::receiveFrame()
{
    const int FRAME_SIZE = Frame::FRAME_SIZE;  // 예상 프레임 크기
    static QByteArray buffer;       // 수신 데이터를 저장하는 버퍼

    // 현재 수신된 데이터를 계속 읽음
    while (tcpSocket->bytesAvailable() > 0) {
        int remainingData = FRAME_SIZE - buffer.size(); // 남은 데이터 계산
        buffer.append(tcpSocket->read(remainingData));  // 남은 데이터만큼 읽기

        // 프레임 데이터가 다 채워졌을 경우
        if (buffer.size() == FRAME_SIZE) {
            qDebug() << "프레임 수신 완료. 크기:" << buffer.size();

            // 프레임을 처리
            processFrame(buffer);

            // 버퍼 초기화
            buffer.clear();
        }
    }
}

void VideoStreamWidget::processFrame(const QByteArray& frameData)
{
    if (frameData.size() != Frame::FRAME_SIZE) {
        qWarning() << "Invalid frame data size: expected" << Frame::FRAME_SIZE << ", got" << frameData.size();
        return;
    }

    QElapsedTimer timer;
    timer.start();

    // OpenCV Mat 생성 및 데이터 변환
    cv::Mat frame(Frame::HEIGHT, Frame::WIDTH, CV_8UC3, (uchar*)frameData.data());
    if (frame.empty()) {
        qWarning() << "Failed to decode frame!";
        return;
    }

    // BGR -> RGB 변환
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    if (isFullScreen && fullScreenVideoLabel) {
        fullScreenVideoLabel->setPixmap(QPixmap::fromImage(img).scaled(fullScreenWindow->size(), Qt::KeepAspectRatio));
    } else {
        ui->videoLabel->setPixmap(QPixmap::fromImage(img).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
        qDebug() << " shot ";
        QThread::sleep(1);
    }

    qDebug() << "Frame processing time:" << timer.elapsed() << "ms";
}

void VideoStreamWidget::playVideo()
{
    // 비디오 재생
    if (!timer->isActive()) {
        timer->start(33);  // 100ms마다 UI 갱신
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

void VideoStreamWidget::toggleFullScreen()
{
    if (!isFullScreen) {
        // 전체 화면 창 생성
        fullScreenWindow = new QWidget();
        fullScreenWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

        // 화면 크기 설정
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        fullScreenWindow->setGeometry(screenGeometry);

        // 전체 화면 라벨 생성
        fullScreenVideoLabel = new QLabel(fullScreenWindow);
        fullScreenVideoLabel->setAlignment(Qt::AlignCenter);
        fullScreenVideoLabel->setGeometry(fullScreenWindow->rect());

        // OpenCV 프레임으로 전체 화면 라벨 업데이트
        if (!currentFrame.empty()) {
            QImage img((const uchar*)currentFrame.data, currentFrame.cols, currentFrame.rows, currentFrame.step, QImage::Format_BGR888);
            fullScreenVideoLabel->setPixmap(QPixmap::fromImage(img));
        }

        // 전체 화면 모드에서 닫기 버튼 추가
        QPushButton *closeButton = new QPushButton("닫기", fullScreenWindow);
        closeButton->setGeometry(10, 10, 100, 40);  // 버튼 위치와 크기 설정
        connect(closeButton, &QPushButton::clicked, this, &VideoStreamWidget::toggleFullScreen); // 클릭 시 전체 화면 종료

        fullScreenWindow->show();
        isFullScreen = true;
        qDebug() << "Entering Fullscreen Mode";
    } else {
        // 전체 화면 해제
        if (fullScreenWindow) {
            fullScreenWindow->close();
            delete fullScreenWindow;
            fullScreenWindow = nullptr;
        }
        isFullScreen = false;
        qDebug() << "Exiting Fullscreen Mode";
    }
}

void VideoStreamWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && isFullScreen) {
        toggleFullScreen(); // ESC 키로 전체 화면 종료
        event->accept(); // ESC 키 이벤트 처리 완료
    } else {
        QWidget::keyPressEvent(event); // 기본 동작 유지
    }
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
