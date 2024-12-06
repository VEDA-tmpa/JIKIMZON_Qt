#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>

#include "videostreamplayer.h"
#include "metadatadisplay.h"
#include "eventlogmanager.h" // EventLogManager 헤더 포함

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
    void on_searchButton_clicked();
    void loadEventLogs();
    void onJsonReadyRead();
    void toggleMode();
    void setLightMode();
    void setDarkMode();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    QTcpSocket *jsonSocket;
    VideoStreamPlayer *player;

    MetaDataDisplay *metaData;

    EventLogManager *eventLogManager; // EventLogManager 포인터 추가
    QStandardItemModel *model; // QTableView에 사용할 모델

    bool isNightMode; // 현재 모드 상태 (낮/밤)
};

#endif // MAINWINDOW_H
