#ifndef EVENTLOGWIDGET_H
#define EVENTLOGWIDGET_H

#include <QWidget>
#include <QObject>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QJsonObject>
#include <QStandardItemModel>

namespace Ui {
class EventLogWidget;
}

class EventLogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EventLogWidget(QWidget *parent = nullptr);
    ~EventLogWidget();

    void onsearchButtonclicked();
    void loadEventLogs();


private:
    Ui::EventLogWidget *ui;

    QSqlDatabase db;
    QStandardItemModel *model; // QTableView에 사용할 모델
};

#endif // EVENTLOGWIDGET_H
