#include "mainwindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>

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

    // 파일을 선택하는 예시 버튼 (사용자가 파일을 선택하고 처리)
    QString filePath = QFileDialog::getOpenFileName(this, "Open Video File", "", "All Files (*)");
    if (!filePath.isEmpty()) {
        player->processFile(filePath);  // 비디오 파일 처리 시작
    }
}

MainWindow::~MainWindow() {
    delete videoLabel;
    delete player;
}

void MainWindow::onFrameReady(const QImage &image) {
    videoLabel->setPixmap(QPixmap::fromImage(image));  // QLabel에 비디오 프레임 표시
}
