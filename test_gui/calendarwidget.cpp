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
    db.setDatabaseName("/Volumes/jjeongni/QtProgramming/JIKIMZON_Qt/test_gui/event_log.db");

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
        qDebug() << "setupCalendar Query failed:" << query.lastError().text();  // 쿼리 실패 시 출력
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
    QChart *chart = new QChart();
    chart->setTitle("날짜별 탐지 객체 수");

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 50);
    axisY->setTickInterval(1);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->barGraphLayout->addWidget(chartView);
}

void CalendarWidget::onDateSelected(const QDate &date) {
    QString selectedDate = date.toString("yyyyMMdd");
    qDebug() << "Selected Date:" << selectedDate;

    updateBarGraphForDate(selectedDate);  // 선택한 날짜에 따라 막대그래프 갱신
}

void CalendarWidget::updateBarGraphForDate(const QString &date) {
    qDebug() << "Updating bar graph for date:" << date;

    // QMap에 objectType별 개수를 저장
    QMap<QString, int> objectCounts;

    // 데이터베이스 쿼리 준비
    QSqlQuery query;
    QString queryString = "SELECT object_class, COUNT(*) FROM event_logs WHERE timestamp LIKE :datePattern GROUP BY object_class";

    // 쿼리 준비 및 실행
    query.prepare(queryString);

    // 날짜 패턴에 "%"를 추가하여 바인딩
    QString datePattern = date + "%"; // 예: "20241209%"
    query.bindValue(":datePattern", datePattern);

    // 쿼리 실행 및 결과 확인
    if (!query.exec()) {
        qDebug() << "Query failed:" << query.lastError().text();
        return;
    }

    // 결과를 QMap에 저장
    while (query.next()) {
        QString objectType = query.value(0).toString(); // object_type 열
        int count = query.value(1).toInt();            // COUNT(*) 열
        objectCounts[objectType] = count;
    }

    // 막대 그래프 업데이트 준비
    QBarSeries *series = new QBarSeries();
    QStringList categories;
    QMap<QString, QString> emojis;

    // 이모티콘과 색상 설정
    emojis["biodegradable"] = "🌱";
    emojis["cardboard"] = "📦";
    emojis["glass"] = "🍾";
    emojis["metal"] = "🔩";
    emojis["paper"] = "📄";
    emojis["plastic"] = "🧴";

    // 핑크 계열 색상 설정
    QColor lightPink = QColor(245, 181, 185); // 연한 핑크

    QBarSet *set = new QBarSet("Objects");

    set->setColor(lightPink); // 막대 색상 설정

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
