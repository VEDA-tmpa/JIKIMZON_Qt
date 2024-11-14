#include "widget.h"
#include "ui_widget.h"  // UI 파일로부터 생성된 헤더 파일

Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget), frameReady(false)
{
    ui->setupUi(this);  // UI 구성 요소 초기화

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Widget::readVideoStream);

    tcpSocket->connectToHost("127.0.0.1", 9999);  // 서버와 연결

    // QTimer를 설정하여 일정 주기로 UI를 업데이트
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::updateUI);

    // 100ms 또는 200ms마다 UI를 갱신
    timer->start(100);  // 100ms마다 UI를 갱신
}

Widget::~Widget()
{
    delete ui;
}

void Widget::readVideoStream()
{
    static const int headerSize = sizeof(quint32);

    // 헤더 데이터를 먼저 읽기
    if (tcpSocket->bytesAvailable() < headerSize) {
        return;
    }

    QByteArray headerData = tcpSocket->read(headerSize);
    quint32 frameSize = *(reinterpret_cast<quint32*>(headerData.data()));

    // 프레임 데이터가 모두 도착할 때까지 기다림
    if (tcpSocket->bytesAvailable() < frameSize) {
        if (!tcpSocket->waitForReadyRead(100)) { // 100ms 대기
            return; // 데이터가 도착하지 않으면 반환
        }
    }

    // 프레임 데이터를 모두 읽기
    QByteArray frameData = tcpSocket->read(frameSize);
    std::vector<uchar> buffer(frameData.begin(), frameData.end());

    // OpenCV로 이미지 디코딩
    cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

    if (!frame.empty()) {
        // 프레임을 저장해 놓고 UI 갱신에서 처리
        currentFrame = frame;
        frameReady = true;  // 새로운 프레임이 준비됨을 표시
    }
}

void Widget::updateUI()
{
    if (frameReady && !currentFrame.empty()) {
        // OpenCV 이미지를 QImage로 변환
        QImage img((const uchar*)currentFrame.data, currentFrame.cols, currentFrame.rows, currentFrame.step, QImage::Format_BGR888);

        // UI에서 비디오를 업데이트
        ui->videoLabel->setPixmap(QPixmap::fromImage(img));

        // 프레임을 하나 처리했으므로, 이제 frameReady 플래그를 리셋
        frameReady = false;
    }
}
