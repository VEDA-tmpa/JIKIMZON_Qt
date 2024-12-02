#include "metadatadisplay.h"
#include "ui_metadatadisplay.h"

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

void MetaDataDisplay::updateMetaData(const QString& time, const QString& location, const QString& objectType) {
    // // 타임스탬프 처리 (시간 형식으로 변환)
    // QString time = QString::fromStdString(detection.timeStamp);  // 필요한 경우 형식을 맞춰서 변환
    // ui->timeLabel->setText("시간: " + time);

    // // 위치 처리 (detection.box의 좌표 (x, y) 사용)
    // QString location = QString("위치: (%1, %2)").arg(detection.box.x).arg(detection.box.y);
    // ui->locationLabel->setText(location);

    // // 객체 타입 처리 (className을 사용)
    // QString objectType = QString::fromStdString(detection.className);
    // ui->typeLabel->setText("객체 종류: " + objectType);

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
                        "   border: 3px solid %1;"  // 테두리 색상
                        "   border-radius: 8px;"    // 모서리 둥글게 설정
                        "   padding: 5px;"          // 패딩 추가
                        "}"
                        ).arg(borderColor);
    ui->iconLabel->setStyleSheet(style);

    ui->eventLog->addItem(time + " - " + objectType + " - " + location);
    ui->eventLog->setMinimumSize(300, 500);
}
