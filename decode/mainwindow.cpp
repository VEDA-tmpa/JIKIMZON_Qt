#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "videostreamplayer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    VideoStreamPlayer* videoStreamPlayer = new VideoStreamPlayer();
    videoStreamPlayer->InitStreamPlayer("192.168.50.14", 12345, 1280, 720, 1000000, 15);
    videoStreamPlayer->RunStreamPlayer();
}

MainWindow::~MainWindow()
{
    delete ui;
}
