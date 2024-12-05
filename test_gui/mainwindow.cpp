#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "metadatadisplay.h"
#include "Frame.h"
#include <QLayout>
#include <QPalette>
#include <QMessageBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include <QSqlRecord> // QSqlRecord 헤더 포함
#include <QImage>
#include <QPixmap>
#include <QObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , tcpSocket(new QTcpSocket(this))  // 소켓 초기화
    , jsonSocket(new QTcpSocket(this))
    , player(new VideoStreamPlayer(nullptr, this))  // 플레이어 초기화
{
    ui->setupUi(this);

    // 초기 모드는 Light Mode로 설정
    setLightMode();

    //테마 버튼
    connect(ui->btnToggleMode, &QPushButton::clicked, this, &MainWindow::toggleMode);
    // 버튼 초기 아이콘 설정 (해 모양)
    ui->btnToggleMode->setIcon(QIcon(":/icon/sun.png"));
    ui->btnToggleMode->setIconSize(QSize(20, 20));

    // TCP 소켓 연결
    tcpSocket->connectToHost("192.168.50.14", 23456);
    tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

    // VideoStreamPlayer와 UI 연결
    connect(player, &VideoStreamPlayer::frameReady, this, [&](const QImage &frame) {
        ui->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    });

    jsonSocket->connectToHost("192.168.50.14", 54321);
    connect(player, &VideoStreamPlayer::parseObjectDetectionData, this, &MainWindow::onJsonReadyRead);
    //connect(jsonSocket, &MetaDataDisplay::updateMetaData, player, &MainWindow::onJsonReadyRead);

    //비디오 스트림 버튼 연결
    connect(ui->pauseButton, &QPushButton::clicked, this, [&]() {
        player->pauseStream();
    });
    connect(ui->resumeButton, &QPushButton::clicked, this, [&]() {
        player->resumeStream();
    });
    connect(ui->backwardButton, &QPushButton::clicked, player, &VideoStreamPlayer::goBackward);
    connect(ui->forwardButton, &QPushButton::clicked, player, &VideoStreamPlayer::goForward);

    // 스트림 시작
    player->startStream(tcpSocket, 1280, 720, 1280 * 720 * 3);

    // MetaDataDisplay 생성
    MetaDataDisplay* metaData = new MetaDataDisplay(this);

    // metaDataContainer에 MetaDataDisplay 추가
    if (ui->metaDataContainer->layout()) {
        ui->metaDataContainer->layout()->addWidget(metaData);
    } else {
        // 레이아웃이 없는 경우 새로 설정
        QVBoxLayout* layout = new QVBoxLayout(ui->metaDataContainer);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(metaData);
        ui->metaDataContainer->setLayout(layout);
    }

    // 메타 데이터 업데이트
    //metaData->updateMetaData();

    eventLogManager = new EventLogManager("event_log.db", this);
    qDebug() << "EventLogManager 초기화 완료";

    // QTableView에 사용할 모델 생성
    model = new QStandardItemModel(this);
    ui->eventlogtableView->setModel(model); // 테이블 뷰에 모델 설정
    qDebug() << "테이블 뷰 모델 설정 완료";

    //검색 버튼 클릭 시 슬롯 연결
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
    qDebug() << "검색 버튼 시그널 연결 완료";

}

void MainWindow::on_searchButton_clicked() {
    qDebug() << "on_searchButton_clicked 호출";

    QString searchTerm = ui->eventlineEdit->text(); // QLineEdit에서 검색어 가져오기
    QString selectedValue = ui->eventcomboBox->currentText(); // 콤보박스에서 선택된 값 가져오기

    qDebug() << "검색어: " << searchTerm << ", 선택값: " << selectedValue;

    // 데이터베이스에서 해당 값을 검색
    QString query = QString("SELECT * FROM event_logs WHERE object_class LIKE '%%1%' AND object_class = '%2'")
                        .arg(searchTerm)
                        .arg(selectedValue);

    model->clear(); // 이전 데이터 지우기
    model->setHorizontalHeaderLabels({"ID", "Frame ID", "Timestamp", "Object Class", "X", "Y", "Width", "Height"}); // 헤더 설정

    QSqlQuery sqlQuery(query);

    if (sqlQuery.exec()) {
        qDebug() << "SQL Query 실행 성공";
        while (sqlQuery.next()) {
            QList<QStandardItem*> rowItems;
            for (int i = 0; i < sqlQuery.record().count(); ++i) {
                rowItems.append(new QStandardItem(sqlQuery.value(i).toString()));
            }
            model->appendRow(rowItems); // 모델에 행 추가
        }
        qDebug() << "검색 결과 처리 완료";
    } else {
        qDebug() << "SQL Query 실행 실패: " << sqlQuery.lastError().text();
        QMessageBox::warning(this, "Error", "Failed to execute query: " + sqlQuery.lastError().text());
    }
}
void MainWindow::toggleMode() {
    isNightMode = !isNightMode;

    if (isNightMode) {
        setDarkMode();
        ui->btnToggleMode->setIcon(QIcon("qrc:/icon/moon.png")); // 밤 모드 아이콘
    } else {
        setLightMode();
        ui->btnToggleMode->setIcon(QIcon("qrc:/icon/sun.png")); // 낮 모드 아이콘
    }
}

void MainWindow::setLightMode() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#ffffff"));
    palette.setColor(QPalette::WindowText, QColor("#000000"));
    palette.setColor(QPalette::Base, QColor("#f5f5f5"));
    palette.setColor(QPalette::Button, QColor("#e0e0e0"));
    palette.setColor(QPalette::ButtonText, QColor("#000000"));
    qApp->setPalette(palette);
}

void MainWindow::setDarkMode() {
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#2b2b2b"));
    palette.setColor(QPalette::WindowText, QColor("#ffffff"));
    palette.setColor(QPalette::Base, QColor("#3b3b3b"));
    palette.setColor(QPalette::Button, QColor("#444444"));
    palette.setColor(QPalette::ButtonText, QColor("#ffffff"));
    qApp->setPalette(palette);
}

MainWindow::~MainWindow()
{
    // 리소스 정리
    player->stopStream();
    delete ui;
}

void MainWindow::onJsonReadyRead()
{
    QByteArray jsonData = jsonSocket->readAll();
    QString jsonString(jsonData);

}

