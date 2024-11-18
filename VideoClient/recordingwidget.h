#ifndef RECORDINGWIDGET_H
#define RECORDINGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>
#include "VideoStreamWidget.h"

class RecordingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RecordingWidget(VideoStreamWidget *videoStream, QWidget *parent = nullptr);
    ~RecordingWidget();

private slots:
    void startStopRecording();
    void captureSnapshot();
    void setSaveLocation();

private:
    void initializeUI();
    void saveFrame(const cv::Mat &frame);

    QPushButton *recordButton;
    QPushButton *snapshotButton;
    QPushButton *saveLocationButton;

    bool isRecording;
    QString savePath;
    cv::VideoWriter videoWriter;
    VideoStreamWidget *videoStreamWidget;  // VideoStreamWidget 객체
};

#endif // RECORDINGWIDGET_H
