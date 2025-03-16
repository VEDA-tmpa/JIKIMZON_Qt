#ifndef JIKIMZON_MAINWINDOW_H
#define JIKIMZON_MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>
#include <QSqlRecord>
#include <QPalette>
#include <QImage>
#include <QPixmap>

#include "videostreamplayer.h"
#include "metadatadisplay.h"
#include "eventlogmanager.h"

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

    void InitMainWindow();

public slots:
    void onsearchButtonclicked(); // 검색 버튼 클릭 시 슬롯

private slots:
    void toggleMode();
    void setLightMode();
    void setDarkMode();

    void onBrightnessSliderChanged(int value);//밝기
    void onSaturationSliderChanged(int value);//채도
    void onSharpnessSliderChanged(int value);//선명도
    void applyBrightnessEffect(QImage &image, int brightness);
    void applySaturationEffect(QImage &image, int saturation);
    void applySharpnessEffect(QImage &image, int sharpness);

private:
    Ui::MainWindow *mUI;

    VideoStreamPlayer *mVideoStreamPlayer;
    MetaDataDisplay *mMetaData;
    // EventLogManager *mEventLogManager; // EventLogManager 포인터 추가
    QStandardItemModel *mItemModel; // QTableView에 사용할 모델

    bool mbNightMode;
    QImage mCurrentFrame;
};
#endif // JIKIMZON_MAINWINDOW_H
