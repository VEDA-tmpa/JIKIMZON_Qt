#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    void initialize();
    QTcpSocket *tcpSocket;

private slots:
    void connectButton();
    void readMessage(); // 서버로부터 메세지를 받을 때 호출 됨
    void disconnected();
};

#endif // WIDGET_H
