#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "videostreamplayer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitStreamPlayer(QString ip, int port, int width, int height, int bitrate, int fps)
{
    mVideoStreamPlayer = new VideoStreamPlayer();
    mVideoStreamPlayer->InitStreamPlayer(ip, port, width, height, bitrate, fps);
}

void MainWindow::RunStreamPlayer()
{
    mVideoStreamPlayer->StartStream();
}