#include "calendarwidget.h"
#include "ui_calendarwidget.h"

CalendarWidget::CalendarWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CalendarWidget)
{
    ui->setupUi(this);

    setupBarGraph();
    setupCalendar();

    // 데이터베이스 연결 초기화
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Volumes/jjeongni/QtProgramming/test_gui/event_log.db");

    if (!db.open()) {
        qDebug() << "Error: " << db.lastError().text();
        return;
    }
}

CalendarWidget::~CalendarWidget()
{
    // 데이터베이스 연결 종료
    QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
    delete ui;
}

void CalendarWidget::setupCalendar() {
    // 캘린더 위젯 생성
    // 캘린더 위젯 생성
    QCalendarWidget *calendar = new QCalendarWidget(this);
    calendar->setFirstDayOfWeek(Qt::Sunday); // 주의 첫 번째 날을 일요일로 설정
    calendar->setGridVisible(true);  // 그리드 보기 설정
    calendar->setMinimumWidth(300);  // 너비를 충분히 넓게 설정
    calendar->setMinimumHeight(300); // 높이도 설정
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader); // 주 번호 숨기기

    // 데이터베이스에서 날짜별 탐지 수를 조회하는 쿼리
    QSqlQuery query;
    query.prepare("SELECT timestamp, count(*) FROM detections GROUP BY timestamp");
    if (query.exec()) {
        while (query.next()) {
            QString timestamp = query.value(0).toString();
            int count = query.value(1).toInt();

            // timestamp에서 날짜 부분만 추출 (yyyyMMdd 형식)
            QString dateStr = timestamp.left(8);  // 예: "20241126"
            QDate calendarDate = QDate::fromString(dateStr, "yyyyMMdd");
            QTextCharFormat format;
            format.setToolTip(QString("탐지 수: %1").arg(count));  // 탐지 수 툴팁 표시
            calendar->setDateTextFormat(calendarDate, format);  // 캘린더 날짜에 툴팁 설정
        }
    } else {
        qDebug() << "Query failed:" << query.lastError().text();  // 쿼리 실패 시 출력
    }

    // 날짜 선택 시 호출되는 슬롯 연결
    connect(calendar, &QCalendarWidget::clicked, this, &CalendarWidget::onDateSelected);

    // 캘린더 위젯을 레이아웃에 추가
    ui->calendarLayout->addWidget(calendar);
    calendar->setStyleSheet(R"(
    QCalendarWidget {
        margin-left: 10px;
        margin-right: 10px;
    }

    /* QCalendarWidget 헤더 스타일 */
    QCalendarWidget QAbstractItemView::header {
        background-color: #f5f5f5;  /* 헤더 배경색 */
        color: #000000;  /* 헤더 텍스트 색 */
        font-weight: bold;  /* 텍스트 굵게 */
        padding: 5px;
    }

    QCalendarWidget QToolTip {
        background-color: #ffffff;
        color: #000000;
    }

    /* 날짜 셀 스타일 */
    QCalendarWidget QDateText {
        color: #000000;
    }
)");

}

void CalendarWidget::setupBarGraph() {

    // 임시 데이터 (날짜별 탐지 객체 수 예시)
    QList<int> sampleData = {5, 10, 15, 30, 45};  // 예시로 5일 간의 탐지 객체 수

    // 그래프 생성 코드
    QBarSet *set = new QBarSet("탐지 객체 수");

    // 예시 데이터를 설정
    for (int value : sampleData) {
        *set << value;  // 각 날짜에 해당하는 탐지 객체 수 추가
    }

    QBarSeries *barSeries = new QBarSeries();
    barSeries->append(set);

    QChart *chart = new QChart();
    chart->addSeries(barSeries);
    chart->setTitle("날짜별 탐지 객체 수");

    // QValueAxis를 사용하여 Y축 범위 수동 설정
    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 50);  // Y축 범위 설정 (최대 값은 필요에 맞게 설정)
    axisY->setTickInterval(1);  // Y축 간격을 1로 설정하여 0.5 단위가 없도록 함

    chart->setAxisY(axisY, barSeries);  // Y축을 수동으로 설정

    // 기본 축 생성 없이 Y축을 수동으로 설정했기 때문에 createDefaultAxes는 호출하지 않음
    chart->createDefaultAxes();  // X축은 자동으로 생성

    // X축을 날짜로 설정 (예시로 간단한 날짜 배열 사용)
    QCategoryAxis *axisX = new QCategoryAxis();
    axisX->append("12-05", 0);
    axisX->append("12-06", 1);
    axisX->append("12-07", 2);
    axisX->append("12-08", 3);
    axisX->append("12-09", 4);  // 날짜 예시 추가
    chart->setAxisX(axisX, barSeries);  // X축 설정

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->barGraphLayout->addWidget(chartView);  // .ui에 설정된 레이아웃에 추가
}

void CalendarWidget::onDateSelected(const QDate &date) {
    QString selectedDate = date.toString("yyyy-MM-dd");
    qDebug() << "Selected Date:" << selectedDate;

    updateBarGraphForDate(selectedDate);  // 선택한 날짜에 따라 막대그래프 갱신
}

void CalendarWidget::updateBarGraphForDate(const QString &date) {
    qDebug() << "Updating bar graph for date:" << date;

    // 데이터베이스에서 선택된 날짜의 탐지 객체를 조회하는 쿼리
    QSqlQuery query;
    query.prepare("SELECT object_type, count(*) FROM detections WHERE timestamp LIKE :date GROUP BY object_type");
    query.bindValue(":date", date + "%");  // 날짜 앞부분을 사용하여 시간대별 탐지 필터링

    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return;
    }

    QMap<QString, int> objectCounts;
    while (query.next()) {
        QString objectType = query.value(0).toString();
        int count = query.value(1).toInt();
        objectCounts[objectType] = count;
    }

    // 막대 그래프 업데이트 준비
    QBarSeries *series = new QBarSeries();
    QStringList categories;
    QMap<QString, QString> emojis;
    QMap<QString, QColor> colors;

    // 이모티콘과 색상 설정
    emojis["biodegradable"] = "🌱";
    emojis["cardboard"] = "📦";
    emojis["glass"] = "🍾";
    emojis["metal"] = "🔩";
    emojis["paper"] = "📄";
    emojis["plastic"] = "🧴";

    colors["biodegradable"] = QColor(138, 201, 38);
    colors["cardboard"] = QColor(255, 89, 94);
    colors["glass"] = QColor(25, 130, 196);
    colors["metal"] = QColor(251, 133, 0);
    colors["paper"] = QColor(255, 202, 58);
    colors["plastic"] = QColor(106, 76, 147);

    QBarSet *set = new QBarSet("Objects");

    int maxValue = 0;
    for (auto it = objectCounts.begin(); it != objectCounts.end(); ++it) {
        *set << it.value();
        categories << emojis[it.key()];
        maxValue = qMax(maxValue, it.value());
    }

    series->append(set);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("날짜별 탐지 객체 수");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, qMax(maxValue, 1)); // maxValue가 0일 경우 1로 설정
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(false);
    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(true);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 기존 그래프 제거 후 새 그래프 추가
    QLayoutItem *oldItem;
    while ((oldItem = ui->barGraphLayout->takeAt(0)) != nullptr) {
        delete oldItem->widget();
        delete oldItem;
    }
    ui->barGraphLayout->addWidget(chartView);
}
