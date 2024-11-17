#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
<<<<<<< HEAD
#include <QStackedWidget>
#include "VideoStreamWidget.h"  // VideoStreamWidget 헤더

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
=======
#include <QVBoxLayout>
#include "VideoStreamWidget.h"
#include "recordingwidget.h"
#include "cameraswitchwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
>>>>>>> feature/switch
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
<<<<<<< HEAD
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // VideoStreamWidget에서 사용할 tcpSocket 생성
    QTcpSocket *tcpSocket;

    // 위젯들
    VideoStreamWidget *videoStreamWidget;
};
=======
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

>>>>>>> feature/switch
#endif // MAINWINDOW_H
