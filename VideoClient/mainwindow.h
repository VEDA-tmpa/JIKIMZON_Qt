#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QVBoxLayout>
#include "VideoStreamWidget.h"
#include "recordingwidget.h"
#include "cameraswitchwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupNetwork();   // 네트워크 연결 초기화 함수
    void initializeUI();   // UI 초기화 함수

    Ui::MainWindow *ui;               // UI 포인터
    QTcpSocket *tcpSocket;            // TCP 소켓
    QWidget *centralWidget;           // 중앙 위젯
    QVBoxLayout *mainLayout;          // 메인 레이아웃

    // 커스텀 위젯
    VideoStreamWidget *videoStreamWidget;
    RecordingWidget *recordingWidget;
    CameraSwitchWidget *cameraSwitchWidget;
};

#endif // MAINWINDOW_H
