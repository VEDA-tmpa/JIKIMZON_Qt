#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include "videostreamplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void InitStreamPlayer(QString ip, int port, int width, int height, int bitrate, int fps);
    void RunStreamPlayer();

private:
    Ui::MainWindow *ui;
    QLabel* mVideoLabel;
    VideoStreamPlayer* mVideoStreamPlayer;
};
#endif // MAINWINDOW_H
