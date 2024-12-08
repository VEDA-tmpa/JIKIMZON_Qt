#include "dashboardwidget.h"
#include "ui_dashboardwidget.h"
#include <QDialog>
#include <QLabel>
#include <QPixmap>
#include <QMessageBox>
#include <QPushButton>
#include <QPainter>
#include <QImage>
#include <QColor>
#include <QListView>
#include <QStringListModel>
#include <QChartView>
#include <QLineSeries>
#include <QChart>
#include <QFile>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QValueAxis>
#include <QDateTimeAxis>

// 초기화
DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DashboardWidget)
    , frameId(0)
{
    ui->setupUi(this);

    // 대시보드 구성
    setupLineChart();
    setupAnomalyList();

    // 타이머 설정 (1초 간격으로 차트 데이터 업데이트)
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DashboardWidget::updateChartData);
    timer->start(1000); // 1000ms = 1초 간격
}

DashboardWidget::~DashboardWidget()
{
    delete ui;
}

// 차트 데이터 업데이트
void DashboardWidget::updateChartData() {
    // 현재 시간 가져오기
    QDateTime currentTime = QDateTime::currentDateTime();

    // 가상 데이터 생성 (실제 데이터 입력으로 변경 가능)
    QList<QString> objectLabels;
    objectLabels << "biodegradable" << "cardboard" << "glass"; // 레이블 데이터

    // 프레임 ID 증가
    frameId++;

    // 객체 탐지 데이터 업데이트
    for (const QString &label : objectLabels) {
        updateDetectedObjects(frameId, currentTime.toString("HH:mm:ss"), label);
    }
}

// 탐지 객체 업데이트
void DashboardWidget::updateDetectedObjects(int frameId, const QString &timestamp, const QString &objectType)
{
    if (objectType.isEmpty()) {
        qWarning() << "Empty objectType received for frame" << frameId;
        return;
    }

    // // 객체 카운트 업데이트
    // objectCounts[objectType]++;

    // // objectCounts의 내용 확인
    // for (auto it = objectCounts.begin(); it != objectCounts.end(); ++it) {
    //     qDebug() << "Category:" << it.key() << "Count:" << it.value();
    // }

    // // 선형 차트 업데이트
    // QColor lineColor = getObjectLineColor(objectType);
    // updateLineChart(objectType, objectCounts[objectType], lineColor);

    // 객체 유형에 따라 카운트를 증가시킴
    if (objectType == "biodegradable") {
        detectedBiodegradableCount++;
    } else if (objectType == "cardboard") {
        detectedCardboardCount++;
    } else if (objectType == "glass") {
        detectedGlassCount++;
    } else if (objectType == "metal") {
        detectedMetalCount++;
    } else if (objectType == "paper") {
        detectedPaperCount++;
    } else if (objectType == "plastic") {
        detectedPlasticCount++;
    }

    // objectCounts의 내용 업데이트 (초기화하지 않고 누적)
    objectCounts["biodegradable"] = detectedBiodegradableCount;
    objectCounts["cardboard"] = detectedCardboardCount;
    objectCounts["glass"] = detectedGlassCount;
    objectCounts["metal"] = detectedMetalCount;
    objectCounts["paper"] = detectedPaperCount;
    objectCounts["plastic"] = detectedPlasticCount;

    // objectCounts의 내용 확인
    qDebug() << "Updating chart. Object counts:" << objectCounts;
    for (auto it = objectCounts.begin(); it != objectCounts.end(); ++it) {
        qDebug() << "Category:" << it.key() << "Count:" << it.value();
    }

    // 선형 차트에 데이터 추가
    for (auto it = objectCounts.begin(); it != objectCounts.end(); ++it) {
        // 객체 유형별로 시리즈 생성 (최초 한 번만 생성)
        if (!objectSeries.contains(it.key())) {
            QLineSeries *series = new QLineSeries();
            series->setName(it.key());
            objectSeries[it.key()] = series;
            chart->addSeries(series);

            // X, Y축을 시리즈에 맞게 설정
            QValueAxis *axisX = new QValueAxis();
            axisX->setRange(0, 60); // X축 0~60초
            QValueAxis *axisY = new QValueAxis();
            axisY->setRange(0, 15); // Y축 0~15
            chart->addAxis(axisX, Qt::AlignBottom);
            series->attachAxis(axisX);
            chart->addAxis(axisY, Qt::AlignLeft);
            series->attachAxis(axisY);
        }

        // `updateLineChart` 호출
        QColor lineColor = getObjectLineColor(it.key());
        updateLineChart(it.key(), it.value(), lineColor);
    }

    // 이상 탐지 목록 업데이트
    updateAnomalyList(frameId, objectCounts.size(), timestamp);

    // 로그 출력
    qDebug() << "Frame" << frameId << ": Detected" << objectCounts[objectType]
             << "objects of type" << objectType;
}

// 객체 유형에 따른 선형 차트 색상 반환
QColor DashboardWidget::getObjectLineColor(const QString &objectType)
{
    static QMap<QString, QColor> colorMap = {
        {"biodegradable", QColor(138, 201, 38)},
        {"cardboard", QColor(255, 89, 94)},
        {"glass", QColor(25, 130, 196)},
        {"metal", QColor(251, 133, 0)},
        {"paper", QColor(255, 202, 58)},
        {"plastic", QColor(106, 76, 147)},
        {"unknown", QColor(128, 128, 128)}
    };

    return colorMap.value(objectType, QColor(128, 128, 128)); // 기본값: 회색
}

// 선형 차트 설정
void DashboardWidget::setupLineChart()
{
    chart = new QChart();
    chart->setTitle("Real-time Object Detection Counts");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // X축 (시간)
    QValueAxis *axisX = new QValueAxis();
    axisX->setTitleText("Time (s)");
     axisX->setRange(0, 60);       // 최근 60초만 표시
    axisX->setLabelFormat("%d"); // 숫자로 표시
    axisX->setTickCount(7);     // 틱 개수 설정
    chart->addAxis(axisX, Qt::AlignBottom);

    // Y축 (객체 개수)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Count");
    axisY->setRange(0, 50);       // 초기 범위 설정
    axisY->setLabelFormat("%d"); // 정수로 표시
    axisY->setTickCount(6);      // 틱 개수 설정
    chart->addAxis(axisY, Qt::AlignLeft);

    // 객체 유형별 시리즈 추가
    QStringList objectTypes = {"biodegradable", "cardboard", "glass", "metal", "paper", "plastic"};
    for (const QString &type : objectTypes) {
        QLineSeries *series = new QLineSeries();

        // 객체 유형에 따라 이모지 설정
        QString emoji;
        if (type == "biodegradable") {
            emoji = "🌱"; // 생분해성
        } else if (type == "cardboard") {
            emoji = "📦"; // 종이박스
        } else if (type == "glass") {
            emoji = "🍾"; // 유리
        } else if (type == "metal") {
            emoji = "🔩"; // 금속
        } else if (type == "paper") {
            emoji = "📄"; // 종이
        } else if (type == "plastic") {
            emoji = "🧴"; // 플라스틱
        }

        // 범례에 이모지 사용
        series->setName(emoji);
        series->setColor(getObjectLineColor(type)); // 사용자 정의 색상 함수
        chart->addSeries(series);

        // 시리즈와 축 연결
        series->attachAxis(axisX);
        series->attachAxis(axisY);

        // 객체 유형과 시리즈 매핑
        objectSeries[type] = series;
    }

    // QChartView 초기화 및 레이아웃 추가
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->lineChartLayout->addWidget(chartView);
}


// 선형 차트 업데이트
void DashboardWidget::updateLineChart(const QString &objectType, int detectedObjectCount, const QColor &lineColor)
{
    if (!objectSeries.contains(objectType)) {
        qWarning() << "No series for object type:" << objectType;
        return;
    }

    QLineSeries *series = objectSeries[objectType];

    // 객체 유형별 시간 관리
    if (!objectTimeMap.contains(objectType)) {
        objectTimeMap[objectType] = 0;  // 초기값 설정
    }

    // 시간 값 갱신 (시리즈별로 고유한 시간 관리)
    int timeInSeconds = objectTimeMap[objectType]++;

    // 시리즈에 데이터 추가
    series->append(timeInSeconds, detectedObjectCount);
    series->setColor(lineColor);

    // X축 및 Y축 동적 범위 업데이트
    QValueAxis *axisX = qobject_cast<QValueAxis *>(chart->axes(Qt::Horizontal).first());
    QValueAxis *axisY = qobject_cast<QValueAxis *>(chart->axes(Qt::Vertical).first());

    if (axisX) {
        // X축을 0에서 시작하도록 설정
        axisX->setRange(0, qMax(timeInSeconds, 60)); // 60초 내에서 표시
    }
    if (axisY) {
        // Y축을 현재 데이터에 맞게 동적으로 확장
        axisY->setRange(0, qMax(axisY->max(), static_cast<qreal>(detectedObjectCount + 5)));
    }
}

// 이상 탐지 리스트 설정
void DashboardWidget::setupAnomalyList()
{
    anomalyListView = new QListView(this);
    anomalyListModel = new QStringListModel(this);
    anomalyListView->setModel(anomalyListModel);

    ui->anomalyListLayout->addWidget(anomalyListView);
}

// 이상 탐지 업데이트
void DashboardWidget::updateAnomalyList(int frameId, int detectedObjectCount, const QString &timestamp)
{
    if (detectedObjectCount >= 10) {
        QApplication::beep();
        QString anomaly = QString("Anomaly Detected! Frame %1: %2 objects at %3")
                              .arg(frameId)
                              .arg(detectedObjectCount)
                              .arg(timestamp);

        QStringList currentList = anomalyListModel->stringList();
        currentList.append(anomaly);
        anomalyListModel->setStringList(currentList);
    }
}
