#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "videostreamplayer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mVideoLabel = new QLabel(this);
    mVideoLabel->setAlignment(Qt::AlignCenter);
    setCentralWidget(mVideoLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitStreamPlayer(QString ip, int port, int width, int height, int bitrate, int fps)
{
    mVideoStreamPlayer = new VideoStreamPlayer();
    mVideoStreamPlayer->InitStreamPlayer(ip, port, width, height, bitrate, fps);
    mVideoStreamPlayer->SetVideoLabel(mVideoLabel);
}

void MainWindow::RunStreamPlayer()
{
    mVideoStreamPlayer->StartStream();
}