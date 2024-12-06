#include "metadatadisplay.h"
#include "ui_metadatadisplay.h"

#include <QDebug>
#include <QPropertyAnimation> // 애니메이션을 위한 헤더 추가

MetaDataDisplay::MetaDataDisplay(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MetaDataDisplay)
{
    ui->setupUi(this);
}

MetaDataDisplay::~MetaDataDisplay()
{
    delete ui;
}

void MetaDataDisplay::updateMetaData(const QString &time, const QString &location, const QString &objectType) {
    qDebug() << "MetaDataDisplay::updateMetaData";

    // 시간 문자열 추출
    QString timeOnly = time.section('_', 1, 1)    // 날짜와 시간 구분: "_" 기준으로 두 번째 부분 추출
                           .section('.', 0, 0);    // 밀리초 제거: "." 기준으로 첫 번째 부분 추출

    // 시간 포맷 변경 (HH:MM:SS로 보기 좋게 변환)
    timeOnly.insert(2, ":").insert(5, ":"); // "114616" -> "11:46:16"


    // UI 레이블에 메타데이터 업데이트
    ui->timeLabel->setText("시간: " + time);
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

    // 애니메이션 추가
    QPropertyAnimation *animation = new QPropertyAnimation(ui->iconLabel, "geometry", this);
    animation->setDuration(300); // 애니메이션 지속 시간 (밀리초)
    animation->setKeyValueAt(0, ui->iconLabel->geometry()); // 초기 상태
    animation->setKeyValueAt(0.5, ui->iconLabel->geometry().adjusted(-10, -10, 10, 10)); // 확대 상태
    animation->setKeyValueAt(1, ui->iconLabel->geometry()); // 원래 상태로 돌아옴
    animation->setEasingCurve(QEasingCurve::OutBounce); // 스프링 효과
    animation->start(QAbstractAnimation::DeleteWhenStopped); // 애니메이션 실행

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
    QString logMessage = QString("%1   %2 %3 (%4)").arg(time, emoji, objectType, location);

    // QListWidget에 추가
    ui->eventLog->addItem(logMessage);
}
