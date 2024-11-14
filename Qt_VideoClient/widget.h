#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QImage>
#include <QTimer>  // QTimer 추가

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void readVideoStream();
    void updateUI();  // UI를 갱신하는 슬롯

private:
    Ui::Widget *ui;          // .ui 파일에서 생성된 UI 관리 포인터
    QTcpSocket *tcpSocket;   // 소켓 포인터
    QTimer *timer;           // UI 업데이트를 위한 타이머
    cv::Mat currentFrame;    // 현재 비디오 프레임
    bool frameReady;         // 새로운 프레임이 준비되었는지 여부
};

#endif // WIDGET_H
