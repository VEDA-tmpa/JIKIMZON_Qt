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
#include <QGraphicsBlurEffect>
#include <QWidget>

#include "videostreamplayer.h"
#include "metadatadisplay.h"
#include "eventlogmanager.h"
#include "dashboardwidget.h"
#include "calendarwidget.h"
#include "sliderdialog.h"
#include "eventlogwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void onsettingsButtonclicked();

private slots:
    //이벤트로그
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
    DashboardWidget *dashboard;
    CalendarWidget *calender;

    EventLogManager *eventLogManager; // EventLogManager 포인터 추가
    EventLogWidget *eventLogWidget; // EventLogWidget UI

    bool isNightMode; // 현재 모드 상태 (낮/밤)

    float brightness = 0.0f;
    float contrast = 1.0f;
    float saturation = 1.0f;
};

#endif // MAINWINDOW_H
