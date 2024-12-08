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
    QChartView *chartView;

};

#endif // METADATADISPLAY_H
