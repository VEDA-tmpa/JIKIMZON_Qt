#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "metadatadisplay.h"
#include "frame.h"
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
#include <QDir>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QtMath> // qRound 사용을 위해 필요

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    // , tcpSocket(new QTcpSocket(this))  // 소켓 초기화
    // , jsonSocket(new QTcpSocket(this))
    , frameSSLSocket(new QSslSocket(this))
    , jsonSSLSocket(new QSslSocket(this))
    , player(new VideoStreamPlayer(this))  // 플레이어 초기화
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/resources/icon.png")); // 리소스 경로에 있는 아이콘 추가
    this->setWindowTitle("JIKIM-ZON"); // 타이틀바 이름 설정

    // 시간 표시 설정
    setupTimeDisplay();

    // 네트워크 상태 확인
    updateNetworkStatus();
    QTimer *networkTimer = new QTimer(this);
    connect(networkTimer, &QTimer::timeout, this, &MainWindow::updateNetworkStatus);
    networkTimer->start(5000);  // 5초마다 네트워크 상태 확인

    setupWeatherDisplay();

    // 초기 모드는 Light Mode로 설정
    setLightMode();

    //테마 버튼
    connect(ui->btnToggleMode, &QPushButton::clicked, this, &MainWindow::toggleMode);

    // 비디오 ssl 소켓 연결
    frameSSLSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    frameSSLSocket->ignoreSslErrors();
    connect(frameSSLSocket, &QSslSocket::encrypted, this, [&]() {
        qDebug() << "Frame SSL connection established.";
    });
    connect(frameSSLSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), 
                     [&](const QList<QSslError> &errors) {
        for (const auto &error : errors)
            qDebug() << "SSL Error:" << error.errorString();
        frameSSLSocket->ignoreSslErrors();
    });
    
    frameSSLSocket->connectToHostEncrypted("192.168.50.14", 1234);
    if (!frameSSLSocket->waitForEncrypted()) {
        qDebug() << "Error:" << frameSSLSocket->errorString();
    }
    

    //json ssl 소켓 연결
    // jsonSSLSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    // connect(jsonSSLSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), 
    //                  [&](const QList<QSslError> &errors) {
    //     for (const auto &error : errors)
    //         qDebug() << "SSL Error:" << error.errorString();
    //     jsonSSLSocket->ignoreSslErrors();
    // });

    // connect(jsonSSLSocket, &QSslSocket::encrypted, this, [&]() {
    //     qDebug() << "JSON SSL connection established.";
    // });

    // jsonSSLSocket->connectToHostEncrypted("192.168.50.14", 4321);
    // if (!jsonSSLSocket->waitForEncrypted()) {
    //     qDebug() << "Error:" << jsonSSLSocket->errorString();
    // }
    // connect(jsonSSLSocket, &QSslSocket::readyRead, this, &MainWindow::onJsonReadyRead);
    


    // VideoStreamPlayer와 UI 연결
    connect(player, &VideoStreamPlayer::frameReady, this, [&](const QImage &frame) {
        ui->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    });

    //비디오 스트림 버튼 연결
    ui->pauseButton->setIcon(QIcon(":/icon/pause.png"));
    ui->resumeButton->setIcon(QIcon(":/icon/play.png"));
    ui->backwardButton->setIcon(QIcon(":/icon/backward.png"));
    ui->forwardButton->setIcon(QIcon(":/icon/forward.png"));

    connect(ui->pauseButton, &QPushButton::clicked, this, [&]() {
        player->pauseStream();
    });
    connect(ui->resumeButton, &QPushButton::clicked, this, [&]() {
        player->resumeStream();
    });
    connect(ui->backwardButton, &QPushButton::clicked, player, &VideoStreamPlayer::goBackward);
    connect(ui->forwardButton, &QPushButton::clicked, player, &VideoStreamPlayer::goForward);

    // 스트림 시작
    player->startStream(frameSSLSocket, 1280, 720, 1280 * 720 * 3);

    // MetaDataDisplay 생성
    metaData = new MetaDataDisplay(this);

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
    connect(player, &VideoStreamPlayer::objectDetected,
            metaData,
            &MetaDataDisplay::updateMetaData);


    eventLogManager = new EventLogManager("/Volumes/jjeongni/QtProgramming/test_gui/event_log.db", this);
    qDebug() << "EventLogManager 초기화 완료";

    player->setEventLogManager(eventLogManager);

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
    QString query = QString("SELECT * FROM event_logs WHERE %1 LIKE '%%2%'")
                        .arg(selectedValue)
                        .arg(searchTerm);

    qDebug() << "Query: " << query;

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

void MainWindow::loadEventLogs() {
    qDebug() << "테이블 초기 데이터 로드 시작";

    // 모델 초기화
    model->clear();
    model->setHorizontalHeaderLabels({"ID", "Frame ID", "Timestamp", "Object Class", "X", "Y", "Width", "Height"});

    // 데이터베이스 쿼리 실행
    QSqlQuery query("SELECT * FROM event_logs");
    if (!query.exec()) {
        qDebug() << "event_logs 테이블 초기 데이터 로드 실패:" << query.lastError().text();
        return;
    }

    while (query.next()) {
        QList<QStandardItem*> rowItems;
        for (int i = 0; i < query.record().count(); ++i) {
            rowItems.append(new QStandardItem(query.value(i).toString()));
        }
        model->appendRow(rowItems);
    }

    ui->eventlogtableView->setModel(model);
    qDebug() << "테이블 초기 데이터 로드 완료";
}


void MainWindow::toggleMode() {
    isNightMode = !isNightMode;

    // opacity effect를 만들어서 아이콘을 변경하는 동안 부드러운 전환 효과를 줄 수 있습니다.
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    ui->btnToggleMode->setGraphicsEffect(effect);

    if (isNightMode) {
        setDarkMode();
        ui->btnToggleMode; // 밤 모드 아이콘
    } else {
        setLightMode();
        ui->btnToggleMode; // 낮 모드 아이콘
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

    QString lightModeStyle = R"(
/* QLabel */
QLabel {
    color: #000000;
    font-size: 14px;
}

/* QListWidget */
QListWidget {
    background-color: #f5f5f5;
    border: 1px solid #e0e0e0;
    color: #000000;
}

/* QTableView */
QTableView {
    background-color: #ffffff;
    gridline-color: #dcdcdc;
    border: 1px solid #e0e0e0;
    color: #000000;
    selection-background-color: #d3e0fc;
    selection-color: #000000;
}

/* QLineEdit */
QLineEdit {
    background-color: #ffffff;
    border: 1px solid #dcdcdc;
    padding: 5px;
    color: #000000;
}

/* QPushButton */
QPushButton {
    background-color: #e0e0e0;
    border: 1px solid #b0b0b0;
    padding: 5px;
    color: #000000;
}

QPushButton:hover {
    background-color: #d6d6d6;
}

QPushButton:pressed {
    background-color: #c0c0c0;
}

/* QComboBox */
QComboBox {
    background-color: #ffffff;
    border: 1px solid #dcdcdc;
    padding: 5px;
    color: #000000;
}

QComboBox::drop-down {
    border-left: 1px solid #dcdcdc;
    background-color: #e0e0e0;
}
/* 낮 모드 스타일 */
QTabWidget {
    background-color: #ffffff; /* 탭 배경 */
    color: #000000; /* 텍스트 색상 */
    border: 1px solid #cccccc;
}

QTabBar::tab {
    background-color: #f5f5f5; /* 탭 색상 */
    border: 1px solid #cccccc; /* 탭 경계 */
    padding: 8px 15px; /* 여백 */
    border-top-left-radius: 5px; /* 라운드 처리 */
    border-top-right-radius: 5px;
    color: #000000;
}

QTabBar::tab:selected {
    background-color: #ffffff; /* 선택된 탭 색상 */
    border-bottom: 1px solid #ffffff; /* 내용과 자연스럽게 연결 */
    font-weight: bold; /* 선택된 탭 강조 */
}

QTabBar::tab:hover {
    background-color: #e0e0e0; /* 탭 위에 마우스 올릴 때 */
}

/* 탭 내용 배경 */
QTabWidget::pane {
    background-color: #ffffff;
    border: 1px solid #cccccc;
    border-top: none; /* 탭 아래쪽 경계 제거 */
}
  )";
     qApp->setStyleSheet(lightModeStyle);

    // metadataDisplay에만 개별 스타일 적용
    ui->metaDataContainer->setStyleSheet("background-color: #f5f5f5; color: #000000; border: 1px solid #e0e0e0;");
}

void MainWindow::setDarkMode() {

    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#2b2b2b"));
    palette.setColor(QPalette::WindowText, QColor("#ffffff"));
    palette.setColor(QPalette::Base, QColor("#3b3b3b"));
    palette.setColor(QPalette::Button, QColor("#444444"));
    palette.setColor(QPalette::ButtonText, QColor("#ffffff"));

    qApp->setPalette(palette);

    // QSS 적용
    QString darkModeStyle = R"(
/* QLabel */
QLabel {
    color: #ffffff;
    font-size: 14px;
}

/* QListWidget */
QListWidget {
    background-color: #3b3b3b;
    border: 1px solid #444444;
    color: #ffffff;
}

/* QTableView */
QTableView {
    background-color: #2b2b2b;
    gridline-color: #444444;
    border: 1px solid #444444;
    color: #ffffff;
    selection-background-color: #555555;
    selection-color: #ffffff;
}

/* QLineEdit */
QLineEdit {
    background-color: #3b3b3b;
    border: 1px solid #444444;
    padding: 5px;
    color: #ffffff;
}

/* QPushButton */
QPushButton {
    background-color: #444444;
    border: 1px solid #555555;
    padding: 5px;
    color: #ffffff;
}

QPushButton:hover {
    background-color: #555555;
}

QPushButton:pressed {
    background-color: #666666;
}

/* QComboBox */
QComboBox {
    background-color: #3b3b3b;
    border: 1px solid #444444;
    padding: 5px;
    color: #ffffff;
}

QComboBox::drop-down {
    border-left: 1px solid #444444;
    background-color: #555555;
}

/* 밤 모드 스타일 */
QTabWidget {
    background-color: #2b2b2b; /* 탭 배경 */
    color: #ffffff; /* 텍스트 색상 */
    border: 1px solid #444444;
}

QTabBar::tab {
    background-color: #3b3b3b; /* 탭 색상 */
    border: 1px solid #444444; /* 탭 경계 */
    padding: 8px 15px; /* 여백 */
    border-top-left-radius: 5px; /* 라운드 처리 */
    border-top-right-radius: 5px;
    color: #ffffff;
}

QTabBar::tab:selected {
    background-color: #2b2b2b; /* 선택된 탭 색상 */
    border-bottom: 1px solid #2b2b2b; /* 내용과 자연스럽게 연결 */
    font-weight: bold; /* 선택된 탭 강조 */
}

QTabBar::tab:hover {
    background-color: #444444; /* 탭 위에 마우스 올릴 때 */
}

/* 탭 내용 배경 */
QTabWidget::pane {
    background-color: #3b3b3b;
    border: 1px solid #444444;
    border-top: none; /* 탭 아래쪽 경계 제거 */
}
    )";
    qApp->setStyleSheet(darkModeStyle);

    // // metadataDisplay에만 개별 스타일 적용
    ui->metaDataContainer->setStyleSheet("background-color: #3b3b3b; color: #000000; border: 1px solid #444444;");
}

// // 시간 표시 업데이트 함수
// void MainWindow::updateTime() {
//     QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm A"); // 년-월-일 시:분 AM/PM
//     ui->timeLabel->setText(currentTime);
// }
// 시간 및 날짜 표시 업데이트 함수
void MainWindow::updateTime() {
    QDateTime now = QDateTime::currentDateTime();

    // 한국어 요일 표시를 위해 QLocale 설정
    QLocale koreanLocale(QLocale::Korean, QLocale::SouthKorea);
    QString currentDate = koreanLocale.toString(now, "yyyy년 MM월 dd일 ddd"); // 한국어 날짜와 요일
    QString currentTime = now.toString("hh:mm A"); // 시간 형식 (AM/PM 포함)

    ui->timeLabel->setText(QString("%1 %2").arg(currentDate).arg(currentTime));
}

// QTimer로 주기적으로 업데이트
void MainWindow::setupTimeDisplay() {
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);
    timer->start(1000);  // 1초마다 업데이트
}

// 네트워크 상태 확인 함수
void MainWindow::updateNetworkStatus() {
    // tcpSocket 상태 확인
    if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
        ui->networkStatusLabel->setText(
            "<span style='color:black;'>연결상태:</span> <span style='color:green; font-weight:bold;'>정상</span>");
    } else {
        ui->networkStatusLabel->setText(
            "<span style='color:black;'>연결상태:</span> <span style='color:red; font-weight:bold;'>끊김</span>");
    }
}

void MainWindow::updateWeather() {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QString apiKey = "84c00c8cadfb7cbf0f63220a6a738d25";
    QString city = "Seoul";
    QString url = QString("http://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric").arg(city, apiKey);

    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, [reply, this]() {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject obj = doc.object();

            double temp = obj["main"].toObject()["temp"].toDouble();
            int roundedTemp = qRound(temp);
            QString description = obj["weather"].toArray()[0].toObject()["description"].toString();
            QString iconCode = obj["weather"].toArray()[0].toObject()["icon"].toString();

            // QMap을 사용하여 한국어로 변환
            QMap<QString, QString> weatherTranslation = {
                {"clear sky", "맑음"},
                {"few clouds", "구름 조금"},
                {"scattered clouds", "흩어진 구름"},
                {"broken clouds", "짙은 구름"},
                {"shower rain", "소나기"},
                {"rain", "비"},
                {"thunderstorm", "뇌우"},
                {"snow", "눈"},
                {"mist", "안개"}
            };
            description = weatherTranslation.value(description, "알 수 없음");

            // 아이콘 다운로드 및 텍스트/아이콘 통합
            QString iconUrl = QString("http://openweathermap.org/img/wn/%1@2x.png").arg(iconCode);
            QNetworkAccessManager *iconManager = new QNetworkAccessManager(this);
            QNetworkRequest iconRequest(iconUrl);
            QNetworkReply *iconReply = iconManager->get(iconRequest);

            connect(iconReply, &QNetworkReply::finished, [iconReply, this, roundedTemp, description]() {
                if (iconReply->error() == QNetworkReply::NoError) {
                    QByteArray iconData = iconReply->readAll();
                    QString base64Data = QString::fromLatin1(iconData.toBase64());
                    QString html = QString(
                                       "<html><body>"
                                       "<img src='data:image/png;base64,%1' width='32' height='32' style='vertical-align:middle;'>"
                                       " 날씨: %2°C, %3"
                                       "</body></html>"
                                       ).arg(base64Data).arg(roundedTemp).arg(description);

                    ui->weatherLabel->setText(html);
                } else {
                    ui->weatherLabel->setText(QString("날씨: %1°C, %2").arg(roundedTemp).arg(description));
                }
                iconReply->deleteLater();
            });
        } else {
            ui->weatherLabel->setText("날씨 정보를 가져올 수 없음");
        }
        reply->deleteLater();
    });
}

void MainWindow::setupWeatherDisplay() {
    updateWeather();  // 처음 실행 시 날씨 표시
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateWeather);
    timer->start(60000);  // 1분마다 날씨 갱신
}


MainWindow::~MainWindow()
{
    // 리소스 정리
    player->stopStream();
    delete ui;
}

void MainWindow::onJsonReadyRead()
{
    QByteArray jsonData = jsonSSLSocket->readAll();

    // 디버깅: 수신된 JSON 데이터 출력
    qDebug() << "Received JSON:" << jsonData;

    // VideoStreamPlayer의 데이터 처리 함수 호출
    player->parseObjectDetectionData(jsonData);

}

