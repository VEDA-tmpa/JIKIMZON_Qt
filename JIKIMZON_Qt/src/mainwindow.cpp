#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
    mUI->setupUi(this);

    // init player
    mVideoStreamPlayer = new VideoStreamPlayer();
    mVideoStreamPlayer->InitStreamPlayer("localhost", 12345, 1280, 720, 100000, 15);

    // connect theme button
    connect(mUI->btnToggleMode, &QPushButton::clicked, this, &MainWindow::toggleMode);
    mUI->btnToggleMode->setIcon(QIcon(":/icon/sun.png"));
    mUI->btnToggleMode->setIconSize(QSize(20, 20));

    // connect player
    connect(mVideoStreamPlayer, &VideoStreamPlayer::FrameReady, this, [&](const QImage& frame) {
        mUI->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(mUI->videoLabel->size(), Qt::KeepAspectRatio));
    });

    // connect video stream buttons
    connect(mUI->pauseButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::PauseStream);
    connect(mUI->resumeButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::ResumeStream);
    connect(mUI->backwardButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::GoBackward);
    connect(mUI->forwardButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::GoForward);

    // meta data display


    // start stream
    if (!mVideoStreamPlayer->isRunning())
    {
        mVideoStreamPlayer->start(QThread::LowPriority);
    }
}

MainWindow::~MainWindow()
{
    mVideoStreamPlayer->StopStream();
    delete mUI;
}

// slots
void MainWindow::toggleMode()
{
    mbNightMode = !mbNightMode;

    if (mbNightMode)
    {
        setDarkMode();
        mUI->btnToggleMode->setIcon(QIcon("qrc:/icon/moon.png")); // 밤 모드 아이콘
    }
    else
    {
        setLightMode();
        mUI->btnToggleMode->setIcon(QIcon("qrc:/icon/sun.png")); // 낮 모드 아이콘
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
