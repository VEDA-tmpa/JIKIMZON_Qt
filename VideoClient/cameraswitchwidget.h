#ifndef CAMERASWITCHWIDGET_H
#define CAMERASWITCHWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <opencv2/opencv.hpp>
#include <vector>

class CameraSwitchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CameraSwitchWidget(QWidget *parent = nullptr);
     ~CameraSwitchWidget();

private slots:
    void switchCamera(int index);
    void updateCameraFeed();

private:
    QComboBox *cameraDropdown;
    QLabel *cameraDisplay;
    std::vector<cv::VideoCapture> cameras;
    QTimer *updateTimer;

    void initializeUI();
    void initializeCameras();
};

#endif // CAMERASWITCHWIDGET_H
