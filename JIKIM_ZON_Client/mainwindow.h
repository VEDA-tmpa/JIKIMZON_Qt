#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QStandardItemModel>
#include "videostreamplayer.h"
#include "decryptor.h"
#include "networkmanager.h"
#include "metadatadisplay.h"
#include "eventlogmanager.h" // EventLogManager 헤더 포함

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
    void onStreamReady();
    void on_searchButton_clicked(); // 검색 버튼 클릭 시 슬롯

    void onBrightnessSliderChanged(int value);//밝기
    void onSaturationSliderChanged(int value);//채도
    void onSharpnessSliderChanged(int value);//선명도

public slots:
    //밤&낮 모드
    void toggleMode();
    void setLightMode();
    void setDarkMode();

    void applyBrightnessEffect(QImage &image, int brightness);
    void applySaturationEffect(QImage &image, int saturation);
    void applySharpnessEffect(QImage &image, int sharpness);


private:
    Ui::MainWindow *ui;

    QLabel *videoLabel;  // 비디오 스트림을 표시할 QLabel
    VideoStreamPlayer *player;
    MetaDataDisplay *metaData;

    bool isNightMode; // 현재 모드 상태 (낮/밤)
    QImage currentFrame;  // 클래스 멤버로 선언하여 사용

    NetworkManager *networkManager;
    EventLogManager *eventLogManager; // EventLogManager 포인터 추가
    QStandardItemModel *model; // QTableView에 사용할 모델

    QByteArray key;  // 암호화 키
    QByteArray nonce;  // nonce

};
#endif // MAINWINDOW_H
