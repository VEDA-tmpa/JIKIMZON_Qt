#include "metadatadisplay.h"
#include "ui_metadatadisplay.h"

#include <QDebug>
#include <QPropertyAnimation> // 애니메이션을 위한 헤더 추가
#include <QGraphicsItem>

MetaDataDisplay::MetaDataDisplay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MetaDataDisplay)
{
    ui->setupUi(this);

    // 차트 초기화
    QChart *chart = new QChart();
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    chart->setTitle("🔍 탐지된 객체 비율");
    QFont titleFont;
    titleFont.setBold(true);
    titleFont.setPointSize(14);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(Qt::darkGray));

    // 기존 UI에 새 레이아웃 추가
    ui->chartLayout->addWidget(chartView);

    // 차트 업데이트
    // updateChart();
}

MetaDataDisplay::~MetaDataDisplay()
{
    delete ui;
}

void MetaDataDisplay::updateMetaData(const QString &time, const QString &location, const QString &objectType) {

    qDebug() << "MetaDataDisplay::updateMetaData";

    // // 시간 문자열 추출
    // QString timeOnly = time.section('_', 1, 1)    // 날짜와 시간 구분: "_" 기준으로 두 번째 부분 추출
    //                        .section('.', 0, 0);    // 밀리초 제거: "." 기준으로 첫 번째 부분 추출

    // // 시간 포맷 변경 (HH:MM:SS로 보기 좋게 변환)
    // timeOnly.insert(2, ":").insert(5, ":"); // "114616" -> "11:46:16"


    // 시간 문자열 추출 및 포맷 변경 (HH:MM)
    QString timeOnly = time.section('_', 1, 1).section('.', 0, 0); // 날짜 제거, 밀리초 제거
    QString timeMinuteOnly = timeOnly.left(4); // "114616" -> "1146"
    timeMinuteOnly.insert(2, ":");           // "1146" -> "11:46"

    // 중복 데이터 체크 (시간 + 객체 유형 기준)
    QString metaDataKey = time + objectType;

    if (processedMetaData.contains(metaDataKey)) {
        qDebug() << "중복 데이터입니다. 업데이트를 건너뜁니다.";
        return;
    }
    processedMetaData.insert(metaDataKey); // 중복 데이터로 추가

    // // UI 레이블에 메타데이터 업데이트
    ui->timeLabel->setText("시간: " + time);
    // ui->timeLabel->setText("시간: " + timeMinuteOnly);
    ui->locationLabel->setText("위치: " + location);
    ui->typeLabel->setText("객체 종류: " + objectType);

    // 아이콘 및 테두리 색상 경로 설정
    QString iconPath;
    QString borderColor;

    // 객체 유형에 따른 아이콘 및 테두리 색상 지정
    if (objectType == "biodegradable") {
        iconPath = ":/detect/biodegradable.png";
        borderColor = "rgb(138, 201, 38)"; // 초록색
    } else if (objectType == "cardboard") {
        iconPath = ":/detect/box.png";
        borderColor = "rgb(255, 89, 94)"; // 빨강색
    } else if (objectType == "glass") {
        iconPath = ":/detect/glass.png";
        borderColor = "rgb(25, 130, 196)"; // 파란색
    } else if (objectType == "metal") {
        iconPath = ":/detect/metal.png";
        borderColor = "rgb(251, 133, 0)"; // 주황색
    } else if (objectType == "paper") {
        iconPath = ":/detect/paper.png";
        borderColor = "rgb(255, 202, 58)"; // 노란색
    } else if (objectType == "plastic") {
        iconPath = ":/detect/plastic.png";
        borderColor = "rgb(106, 76, 147)"; // 보라색
    } else {
        // 정의되지 않은 경우 기본 아이콘과 테두리 색상
        iconPath = ":/detect/default.png";
        borderColor = "rgb(128, 128, 128)"; // 회색
    }

    // 아이콘 설정
    if (!QPixmap(iconPath).isNull()) {
        ui->iconLabel->setPixmap(QPixmap(iconPath).scaled(64, 64, Qt::KeepAspectRatio));
        ui->iconLabel->setAlignment(Qt::AlignCenter); // 아이콘 중앙 배치
    } else {
        // 유효하지 않으면 기본 아이콘 설정
        ui->iconLabel->setPixmap(QPixmap(":/detect/default.png").scaled(64, 64, Qt::KeepAspectRatio));
        ui->iconLabel->setAlignment(Qt::AlignCenter);
    }

    // 테두리 색상 적용 (QSS 사용)
    QString style = QString(
                        "QLabel {"
                        "   background-color: #f0f0f0;"
                        "   border: 3px solid %1;"  // 테두리 색상
                        "   border-radius: 8px;"     // 모서리 둥글게 설정
                        "   padding: 5px;"           // 패딩 추가
                        "}").arg(borderColor);

    ui->iconLabel->setStyleSheet(style); // 스타일 적용

    // 테두리 색상 애니메이션
    QPropertyAnimation *borderAnimation = new QPropertyAnimation(ui->iconLabel, "styleSheet", this);
    borderAnimation->setDuration(500); // 애니메이션 지속 시간 (밀리초)

    // 초기 상태
    QString originalStyle = QString(
                                "QLabel {"
                                "   background-color: #f0f0f0;"
                                "   border: 3px solid %1;"  // 테두리 색상
                                "   border-radius: 8px;"
                                "   padding: 5px;"
                                "}").arg(borderColor);

    // 애니메이션 키프레임
    QString highlightStyle = QString(
                                 "QLabel {"
                                 "   background-color: #f0f0f0;"
                                 "   border: 6px solid %1;"  // 더 두꺼운 테두리
                                 "   border-radius: 10px;"
                                 "   padding: 5px;"
                                 "}").arg(borderColor);

    borderAnimation->setKeyValueAt(0, originalStyle); // 초기 상태
    borderAnimation->setKeyValueAt(0.5, highlightStyle); // 강조 상태
    borderAnimation->setKeyValueAt(1, originalStyle); // 원래 상태로 복구

    borderAnimation->start(QAbstractAnimation::DeleteWhenStopped); // 애니메이션 실행

    connect(borderAnimation, &QPropertyAnimation::finished, [this, originalStyle]() {
        ui->iconLabel->setStyleSheet(originalStyle);
    });

    // 이벤트 로그 추가 (시간 - 객체 종류 - 위치)
    // ui->eventLog->addItem(time + " - " + objectType + " - " + location);
    QString emoji;
    if (objectType == "biodegradable") {
        emoji = "🌱"; // 생분해성
    } else if (objectType == "cardboard") {
        emoji = "📦"; // 종이박스
    } else if (objectType == "glass") {
        emoji = "🍾"; // 유리
    } else if (objectType == "metal") {
        emoji = "🔩"; // 금속
    } else if (objectType == "paper") {
        emoji = "📄"; // 종이
    } else if (objectType == "plastic") {
        emoji = "🧴"; // 플라스틱
    }

    // 로그 메시지 생성
    QString logMessage = QString("%1   %2 %3 (%4)").arg(timeMinuteOnly, emoji, objectType, location);

    // // 로그 메시지 생성
    // QString logMessage = QString("%1   %2  %3").arg(timeMinuteOnly, emoji, objectType);

    // QListWidget에 추가
    ui->eventLog->addItem(logMessage);


    // // 객체 수 업데이트 (분 단위로 카운트)
    // QString currentMinute = timeMinuteOnly; // HH:MM 형식
    // objectCounts[currentMinute][objectType]++; // 분 단위로 객체 타입별 카운트 증가

    // // 현재 시간(분 단위) 체크
    // if (lastUpdatedMinute != currentMinute) {
    //     lastUpdatedMinute = currentMinute;

    //     // 차트 업데이트
    //     updateChart();
    // }

    // 객체 수 업데이트
    objectCounts[objectType]++;

    // 차트 업데이트
    updateChart();
}

void MetaDataDisplay::updateChart()
{
    qDebug() << "Updating chart. Current minute counts:" << objectCounts;

    // // // 테스트를 위한 예시 데이터
    // objectCounts.clear();  // 기존 데이터 초기화
    // objectCounts["biodegradable"] = 0;  // 생분해성 10개
    // objectCounts["cardboard"] = 0;       // 종이박스 5개
    // objectCounts["glass"] = 0;           // 유리 7개
    // objectCounts["metal"] = 0;           // 금속 3개
    // objectCounts["paper"] = 0;           // 종이 8개
    // objectCounts["plastic"] = 0;        // 플라스틱 12개

    // // 현재 시간의 데이터만 가져오기
    // QString currentMinute = lastUpdatedMinute;  // updateMetaData에서 갱신된 lastUpdatedMinute 사용
    // if (!objectCounts.contains(currentMinute)) {
    //     qDebug() << "No data for the current minute:" << currentMinute;
    //     return;
    // }

    // // 현재 시간에 해당하는 객체 데이터를 가져옴
    // QMap<QString, int> currentCounts = objectCounts[currentMinute];

    // qDebug() << "Current counts:" << currentCounts;


    // 도넛 그래프 시리즈 설정
    QPieSeries *series = new QPieSeries();

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

    int totalCount = 0;
    for (auto it = objectCounts.begin(); it != objectCounts.end(); ++it) {
        totalCount += it.value();
    }

    // 탐지된 객체에 대해 비율을 도넛 그래프에 추가
    for (auto it = objectCounts.begin(); it != objectCounts.end(); ++it) {
        QString category = it.key();
        int count = it.value();

        // 객체에 대한 이모티콘을 설정
        QString label = emojis.value(category) + " " + QString::number(count);

        // 도넛 그래프에 추가
        QPieSlice *slice = series->append(label, count);
        slice->setBrush(colors.value(category));  // 색상 설정
        slice->setLabelVisible(true);  // 라벨 표시

        // 비율을 라벨로 추가 (예: "카드보드 15 (25%)")
        slice->setLabel(QString("%1 (%2%)")
                            .arg(emojis.value(category))
                            .arg(static_cast<int>(count * 100.0 / totalCount)));
    }

    // int totalCount = 0;
    // for (auto it = currentCounts.begin(); it != currentCounts.end(); ++it) {
    //     totalCount += it.value();
    // }

    // // 탐지된 객체에 대해 비율을 도넛 그래프에 추가
    // for (auto it = currentCounts.begin(); it != currentCounts.end(); ++it) {
    //     QString category = it.key();
    //     int count = it.value();

    //     // 객체에 대한 이모티콘을 설정
    //     QString label = emojis.value(category) + " " + QString::number(count);

    //     // 도넛 그래프에 추가
    //     QPieSlice *slice = series->append(label, count);
    //     slice->setBrush(colors.value(category));  // 색상 설정
    //     slice->setLabelVisible(true);  // 라벨 표시

    //     // 비율을 라벨로 추가 (예: "카드보드 15 (25%)")
    //     slice->setLabel(QString("%1 (%2%)")
    //                         .arg(emojis.value(category))
    //                         .arg(static_cast<int>(count * 100.0 / totalCount)));
    // }

    connect(series, &QPieSeries::hovered, this, [=](QPieSlice *slice, bool state) {
        if (state) {
            slice->setExploded(true);  // 강조 효과
            slice->setLabelColor(Qt::black);
        } else {
            slice->setExploded(false);
        }
    });

    // 기존 chartView에 새로운 차트 설정
    QChart *chart = chartView->chart();
    chart->removeAllSeries(); // 기존 시리즈 제거
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 마우스 호버 상태에 따른 강조 효과 추가
    connect(series, &QPieSeries::hovered, this, [=](QPieSlice *slice, bool state) {
        if (state) {
            slice->setExploded(true);  // 강조 효과
            slice->setLabelColor(Qt::black); // 라벨 색상 강조
        } else {
            slice->setExploded(false); // 기본 상태
        }
    });

    // 범례 숨기기
    chart->legend()->setVisible(false);

    // 도넛 형태로 만들기 위해 가운데 비워두기
    series->setHoleSize(0.35);  // 가운데 비율을 설정 (0.35는 기본 값)

    chartView->update();  // 차트 뷰 갱신

    // 기존 UI에 새 레이아웃 추가
    ui->chartLayout->addWidget(chartView);
}
