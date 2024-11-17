<<<<<<< HEAD
// RecordingWidget.cpp
=======
>>>>>>> feature/switch
#include "recordingwidget.h"
#include "videostreamwidget.h"

RecordingWidget::RecordingWidget(VideoStreamWidget *videoStream, QWidget *parent)
    : QWidget{parent},
    videoStreamWidget(videoStream),  // VideoStreamWidget 객체 초기화
    isRecording(false),
    savePath(QDir::homePath() + "/Videos")
{
    initializeUI();
}

RecordingWidget::~RecordingWidget() {
    if (isRecording) {
        videoWriter.release();
    }
}

void RecordingWidget::initializeUI() {
    recordButton = new QPushButton("녹화 시작", this);
    snapshotButton = new QPushButton("스냅샷 캡처", this);
    saveLocationButton = new QPushButton("저장 위치 설정", this);

    connect(recordButton, &QPushButton::clicked, this, &RecordingWidget::startStopRecording);
    connect(snapshotButton, &QPushButton::clicked, this, &RecordingWidget::captureSnapshot);
    connect(saveLocationButton, &QPushButton::clicked, this, &RecordingWidget::setSaveLocation);

    auto *layout = new QVBoxLayout(this);
    layout->addWidget(recordButton);
    layout->addWidget(snapshotButton);
    layout->addWidget(saveLocationButton);
    setLayout(layout);
}

void RecordingWidget::startStopRecording() {
    if (isRecording) {
        videoWriter.release();
        recordButton->setText("녹화 시작");
        isRecording = false;
    } else {
        QString videoFile = QFileDialog::getSaveFileName(this, "비디오 파일 저장", savePath, "*.avi");
        if (!videoFile.isEmpty()) {
            videoWriter.open(videoFile.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, cv::Size(640, 480));
            recordButton->setText("녹화 정지");
            isRecording = true;
        }
    }
}

void RecordingWidget::captureSnapshot() {
    QString snapshotFile = QFileDialog::getSaveFileName(this, "스냅샷 저장", savePath, "*.png");
    if (!snapshotFile.isEmpty()) {
<<<<<<< HEAD
        cv::Mat frame = videoStreamWidget->getCurrentFrame();  // VideoStreamWidget에서 현재 프레임 가져오기
        saveFrame(frame);
    }
}

=======
        cv::Mat frame = videoStreamWidget->getCurrentFrame();  // 현재 프레임 가져오기
        if (frame.empty()) {
            qDebug() << "Error: Captured frame is empty.";
            return;
        }

        // 프레임 저장 시 경로 처리
        if (!cv::imwrite(snapshotFile.toStdString(), frame)) {
            qDebug() << "Error: Failed to save snapshot at" << snapshotFile;
        } else {
            qDebug() << "Snapshot saved at" << snapshotFile;
        }
    }
}

void RecordingWidget::saveFrame(const cv::Mat &frame) {
    QString defaultPath = savePath + "/snapshot.png";
    if (!frame.empty()) {
        if (!cv::imwrite(defaultPath.toStdString(), frame)) {
            qDebug() << "Error: Failed to save frame at" << defaultPath;
        } else {
            qDebug() << "Frame saved at" << defaultPath;
        }
    } else {
        qDebug() << "Error: Frame is empty.";
    }
}


>>>>>>> feature/switch
void RecordingWidget::setSaveLocation() {
    QString folder = QFileDialog::getExistingDirectory(this, "저장 위치 설정", savePath);
    if (!folder.isEmpty()) {
        savePath = folder;
    }
}

<<<<<<< HEAD
void RecordingWidget::saveFrame(const cv::Mat &frame) {
    if (!frame.empty()) {
        cv::imwrite(savePath.toStdString() + "/snapshot.png", frame);  // 프레임 저장
    }
}
=======
>>>>>>> feature/switch
