#ifndef CALENDARWIDGET_H
#define CALENDARWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QCalendarWidget>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class CalendarWidget;
}

class CalendarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CalendarWidget(QWidget *parent = nullptr);
    ~CalendarWidget();

private:
    Ui::CalendarWidget *ui;
    QSqlDatabase db;  // 멤버 변수로 데이터베이스 선언

    void setupCalendar();
    void setupBarGraph();

private slots:
    void onDateSelected(const QDate &date);  // 캘린더 날짜 선택 시 호출
    void updateBarGraphForDate(const QString &date);  // 날짜 선택에 따른 막대그래프 업데이트
};

#endif // CALENDARWIDGET_H
