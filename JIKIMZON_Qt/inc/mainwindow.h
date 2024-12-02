#ifndef JIKIMZON_MAINWINDOW_H
#define JIKIMZON_MAINWINDOW_H

#include <QMainWindow>

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

private slots:
    void toggleMode();
    void setLightMode();
    void setDarkMode();

private:
    Ui::MainWindow *mUI;
    VideoStreamPlayer *mVideoStreamPlayer;

    bool mbNightMode;
};
#endif // JIKIMZON_MAINWINDOW_H
