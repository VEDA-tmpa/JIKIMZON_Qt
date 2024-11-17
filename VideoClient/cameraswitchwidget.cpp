#include "cameraswitchwidget.h"
#include <QTimer>

CameraSwitchWidget::CameraSwitchWidget(QWidget *parent)
    : QWidget{parent}
{
    initializeUI();
    // initializeCameras();

    // updateTimer = new QTimer(this);
    // connect(updateTimer, &QTimer::timeout, this, &CameraSwitchWidget::updateCameraFeed);
    // updateTimer->start(30); // 30ms마다 업데이트
}

CameraSwitchWidget::~CameraSwitchWidget() {
    // for (auto &camera : cameras) {
    //     if (camera.isOpened()) {
    //         camera.release();
    //     }
    // }
}

void CameraSwitchWidget::initializeUI() {
    cameraDropdown = new QComboBox(this);
    cameraDisplay = new QLabel(this);

    // connect(cameraDropdown, QOverload<int>::of(&QComboBox::activated), this, &CameraSwitchWidget::switchCamera);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(cameraDropdown);
    layout->addWidget(cameraDisplay);
    setLayout(layout);
}

void CameraSwitchWidget::initializeCameras() {
    // for (int i = 0; i < 4; ++i) { // 최대 4개의 카메라 가정
    //     cv::VideoCapture camera(i);
    //     if (camera.isOpened()) {
    //         cameras.push_back(std::move(camera));
    //         cameraDropdown->addItem("Camera " + QString::number(i));
    //     }
    // }
}

void CameraSwitchWidget::switchCamera(int index) {
    // for (size_t i = 0; i < cameras.size(); ++i) {
    //     if (i == static_cast<size_t>(index)) {
    //         cameras[i].open(i);
    //     } else {
    //         cameras[i].release();
    //     }
    // }
}

void CameraSwitchWidget::updateCameraFeed() {
    // if (cameraDropdown->currentIndex() >= 0 && cameraDropdown->currentIndex() < cameras.size()) {
    //     cv::Mat frame;
    //     cameras[cameraDropdown->currentIndex()].read(frame);

    //     if (!frame.empty()) {
    //         QImage image(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_BGR888);
    //         cameraDisplay->setPixmap(QPixmap::fromImage(image));
    //     }
    // }
}
