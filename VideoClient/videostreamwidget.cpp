#include "VideoStreamWidget.h"
#include "ui_VideoStreamWidget.h"
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QImage>
#include <QPixmap>
#include <cstdint> // uint32_t 사용
#include <QElapsedTimer>
#include "framespecs.h"

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

    // QTimer를 설정하여 일정 주기로 UI를 업데이트
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        QByteArray frame; // 예: 데이터를 멤버 변수 또는 다른 방식으로 준비
        processFrame(frame);
    });

    // 100ms 또는 200ms마다 UI를 갱신
    timer->start(33);  // 100ms마다 UI를 갱신
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
    const int FRAME_SIZE = 2764800;
    static QByteArray buffer;      // 수신 데이터를 저장하는 지속 버퍼
    static int expectedFrameSize = -1; // 현재 프레임 크기, 초기값 -1은 헤더를 아직 읽지 않은 상태를 의미

    while (tcpSocket->bytesAvailable()) {
        // Step 1: 헤더 처리
        if (expectedFrameSize == -1 && buffer.size() < 4) {
            // 헤더 데이터 읽기 (프레임 크기를 나타내는 4바이트)
            buffer.append(tcpSocket->read(4 - buffer.size()));
            if (buffer.size() < 4) {
                return; // 헤더가 아직 완전히 수신되지 않았다면 기다림
            }

            // 헤더에서 예상 프레임 크기 추출
            memcpy(&expectedFrameSize, buffer.data(), 4);
            expectedFrameSize = ntohl(expectedFrameSize); // 네트워크 바이트 순서를 호스트 순서로 변환

            if (expectedFrameSize != FRAME_SIZE) {
                qWarning() << "수신된 프레임 크기가 예상과 다릅니다. 예상:" << FRAME_SIZE
                           << ", 수신:" << expectedFrameSize;
                buffer.clear();       // 프레임 크기가 유효하지 않으면 버퍼 초기화
                expectedFrameSize = -1; // 헤더 상태를 초기화
                return;
            }

            buffer.remove(0, 4); // 버퍼에서 헤더 데이터 제거
        }

        // Step 2: 프레임 데이터 처리
        int remainingData = expectedFrameSize - buffer.size();
        if (remainingData > 0) {
            // 프레임 데이터를 완전히 수신할 때까지 데이터를 버퍼에 추가
            buffer.append(tcpSocket->read(remainingData));
        }

        // Step 3: 버퍼에 프레임 데이터가 모두 채워졌을 때
        if (buffer.size() == expectedFrameSize) {
            qDebug() << "프레임 수신 완료. 크기:" << buffer.size();
            processFrame(buffer);    // 수신된 프레임을 처리
            buffer.clear();          // 다음 프레임을 위해 버퍼 초기화
            expectedFrameSize = -1;  // 다음 프레임 헤더를 읽기 위해 초기화
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

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    // UI 업데이트
    QPixmap pixmap = QPixmap::fromImage(img);
    QMetaObject::invokeMethod(this, [this, pixmap]() {
        if (isFullScreen && fullScreenVideoLabel) {
            fullScreenVideoLabel->setPixmap(pixmap.scaled(fullScreenWindow->size(), Qt::KeepAspectRatio));
        } else {
            ui->videoLabel->setPixmap(pixmap.scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
        }
    }, Qt::QueuedConnection);

    qDebug() << "Frame processing time:" << timer.elapsed() << "ms";

    frameReady = true;
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
