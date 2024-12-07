#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include <QSslSocket>
#include <QTimer>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSslSocket>

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
    //이벤트로그
    void on_searchButton_clicked();
    void loadEventLogs();
    void onJsonReadyRead();
    //밤, 낮 모드 전환
    void toggleMode();
    void setLightMode();
    void setDarkMode();
    //비디오 상단바
    void updateTime();
    void setupTimeDisplay();
    void updateNetworkStatus();
    void updateWeather();
    void setupWeatherDisplay();

private:
    Ui::MainWindow *ui;
    // QTcpSocket *tcpSocket;
    // QTcpSocket *jsonSocket;
    QSslSocket *frameSSLSocket;
    QSslSocket *jsonSSLSocket;
    QSslConfiguration sslConfig;

    VideoStreamPlayer *player;

    MetaDataDisplay *metaData;

    EventLogManager *eventLogManager; // EventLogManager 포인터 추가
    QStandardItemModel *model; // QTableView에 사용할 모델

    bool isNightMode; // 현재 모드 상태 (낮/밤)
};

#endif // MAINWINDOW_H
