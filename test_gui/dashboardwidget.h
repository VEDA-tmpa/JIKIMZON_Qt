#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QValueAxis>
#include <QListView>
#include <QStringListModel>
#include <QMap>
#include <QListWidget>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

    namespace Ui {
    class DashboardWidget;
}

class DashboardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    ~DashboardWidget();

    // 탐지 객체 업데이트 함수
    void updateDetectedObjects(int frameId, const QString &timestamp, const QString &objectType);

private slots:
    // 차트 데이터 업데이트
    void updateChartData();

private:
    Ui::DashboardWidget *ui;

    // 대시보드 구성 함수
    void setupLineChart();
    void setupAnomalyList();

    // 차트 업데이트 함수
    void updateLineChart(const QString &objectType, int detectedObjectCount, const QColor &lineColor);

    // 이상 탐지 리스트 업데이트 함수
    void updateAnomalyList(int frameId, int detectedObjectCount, const QString &timestamp);

    // 객체 유형에 따른 선형 차트 색상 반환
    QColor getObjectLineColor(const QString &objectType);

    // UI 요소
    QTimer *timer;                            // 데이터 업데이트 타이머
    QChart *chart;                            // 차트
    QChartView *chartView;                    // 차트 뷰
    QMap<QString, QLineSeries *> objectSeries; // 객체 유형별 데이터 시리즈
    QMap<QString, int> objectCounts;          // 객체 유형별 탐지 카운트

    // 이상 탐지 리스트
    QListView *anomalyListView;               // 이상 탐지 리스트 뷰
    QListWidget *anomalyListWidget;
    QStringListModel *anomalyListModel;       // 이상 탐지 리스트 모델

    int frameId;                              // 프레임 ID

    QMap<QString, int> objectTimeMap;  // 객체 유형별 시간 추적 맵

    // 각 객체 유형에 대한 카운트 변수들
    int detectedBiodegradableCount = 0;
    int detectedCardboardCount = 0;
    int detectedGlassCount = 0;
    int detectedMetalCount = 0;
    int detectedPaperCount = 0;
    int detectedPlasticCount = 0;

};

#endif // DASHBOARDWIDGET_H
