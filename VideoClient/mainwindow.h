#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QStackedWidget>
#include "VideoStreamWidget.h"  // VideoStreamWidget 헤더

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

private:
    Ui::MainWindow *ui;

    // VideoStreamWidget에서 사용할 tcpSocket 생성
    QTcpSocket *tcpSocket;

    // 위젯들
    VideoStreamWidget *videoStreamWidget;
};
#endif // MAINWINDOW_H
