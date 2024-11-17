#ifndef CAMERASWITCHWIDGET_H
#define CAMERASWITCHWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

class CameraSwitchWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraSwitchWidget(QWidget *parent = nullptr);
    ~CameraSwitchWidget();

private:
    // UI 초기화
    void initializeUI();

    // 카메라 초기화
    void initializeCameras();

    // 카메라 전환
    void switchCamera(int index);

    // 카메라 피드 업데이트
    void updateCameraFeed();

    // UI 구성 요소들
    QComboBox *cameraDropdown;  // 카메라 선택 드롭다운
    QLabel *cameraDisplay;      // 카메라 피드 표시 레이블

    // 카메라 객체들 (예시로 최대 4개의 카메라를 가정)
    std::vector<cv::VideoCapture> cameras;

    // 카메라 피드 업데이트 타이머
    QTimer *updateTimer;
};

#endif // CAMERASWITCHWIDGET_H
