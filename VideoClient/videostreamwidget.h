#ifndef VIDEOSTREAMWIDGET_H
#define VIDEOSTREAMWIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QImage>
#include <QTimer>  // QTimer 추가
#include <QPushButton>
#include <QScreen>
#include <QApplication>
#include <QKeyEvent>
#include <QDebug>
#include <QLabel>

namespace Ui {
class VideoStreamWidget;
}

class VideoStreamWidget : public QWidget
{
    Q_OBJECT

public:
    // 생성자에서 tcpSocket을 인자로 받도록 수정
    explicit VideoStreamWidget(QTcpSocket *socket, QWidget *parent = nullptr);
    ~VideoStreamWidget();

<<<<<<< HEAD
=======
    cv::Mat getCurrentFrame();  // 현재 프레임을 반환하는 함수

>>>>>>> feature/switch
protected:
    // ESC 키 처리를 위해 keyPressEvent 재정의
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void readVideoStream();  // 비디오 스트림 읽기
    void updateUI();         // UI 갱신
    void playVideo();        // 재생
    void pauseVideo();       // 일시정지
    void stopVideo();        // 멈춤
    void updateNetworkStatus(QAbstractSocket::SocketState socketState);  // 네트워크 상태 업데이트
    void toggleFullScreen(); // 전체 화면 토글

private:
    Ui::VideoStreamWidget *ui;
    QWidget *fullScreenWindow;     // 전체 화면 창
    QLabel *fullScreenVideoLabel;  // 전체 화면 라벨

    QTcpSocket *tcpSocket;   // QTcpSocket을 멤버로 추가

    QTimer *timer;           // UI 업데이트를 위한 타이머
    cv::Mat currentFrame;    // 현재 비디오 프레임
    bool frameReady;         // 새로운 프레임이 준비되었는지 여부
    bool isFullScreen; // 전체 화면 상태
};

#endif // VIDEOSTREAMWIDGET_H
