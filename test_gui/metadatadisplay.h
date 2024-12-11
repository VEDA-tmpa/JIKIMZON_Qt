#ifndef METADATADISPLAY_H
#define METADATADISPLAY_H

#include <QWidget>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QChart>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QMap>
#include <QStringList>
#include <QColor>
#include <QDebug>
#include <QChartView>
#include <QSet>

namespace Ui {
class MetaDataDisplay;
}

class MetaDataDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataDisplay(QWidget *parent = nullptr);
    ~MetaDataDisplay();

public slots:
    void updateMetaData(const QString &time, const QString &location, const QString &objectType);

    void updateChart();
private:
    Ui::MetaDataDisplay *ui;

    QMap<QString, int> objectCounts;
    // 객체 카운트 데이터 구조
    // QMap<QString, QMap<QString, int>> objectCounts;
    //  QString lastUpdatedMinute; // 클래스 멤버 변수로 선언
    QSet<QString> processedMetaData; // 중복 체크를 위한 데이터 구조

    QChartView *chartView;

};

#endif // METADATADISPLAY_H
