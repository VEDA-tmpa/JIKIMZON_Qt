#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    videoLabel(new QLabel(this)),
    player(new VideoStreamPlayer(this)) {

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(videoLabel);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(player, &VideoStreamPlayer::frameReady, this, &MainWindow::onFrameReady);
    connect(player, &VideoStreamPlayer::streamReady, this, &MainWindow::onStreamReady);

    QString filePath = QFileDialog::getOpenFileName(this, "Open Video File", "", "All Files (*)");
    if (!filePath.isEmpty()) {
        player->processFile(filePath);
    }
}

MainWindow::~MainWindow() {
    delete videoLabel;
    delete player;
}

void MainWindow::onFrameReady(const QImage &image) {
    if (!image.isNull()) {
        videoLabel->setPixmap(QPixmap::fromImage(image));  // QLabel에 비디오 프레임 표시
        videoLabel->update(); // 화면 갱신
    } else {
        qDebug() << "Received an invalid image!";
    }
}

void MainWindow::onStreamReady() {
    // 스트림 준비가 완료되면 비디오 레이블을 표시합니다.
    videoLabel->setVisible(true);
}
