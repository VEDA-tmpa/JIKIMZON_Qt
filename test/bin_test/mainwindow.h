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
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onFrameReady(const QImage &image);

private:
    Ui::MainWindow *ui;

    QLabel *videoLabel;  // 비디오 스트림을 표시할 QLabel
    VideoStreamPlayer *player;
};
#endif // MAINWINDOW_H
