#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videostreamplayer.h"
#include "metadatadisplay.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleMode();
    void setLightMode();
    void setDarkMode();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    VideoStreamPlayer *player;

    bool isNightMode; // 현재 모드 상태 (낮/밤)
};

#endif // MAINWINDOW_H
