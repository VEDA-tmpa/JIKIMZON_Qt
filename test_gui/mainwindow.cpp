#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "metadatadisplay.h"
#include "dashboardwidget.h"
#include "sliderdialog.h"
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
#include <QFrame>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , frameSSLSocket(new QSslSocket(this))  // 소켓 초기화
    , jsonSSLSocket(new QSslSocket(this))
    , player(new VideoStreamPlayer(this))  // 플레이어 초기화
{
    ui->setupUi(this);

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

    // //테마 버튼
    // connect(ui->btnToggleMode, &QPushButton::clicked, this, &MainWindow::toggleMode);

    // // VideoStreamPlayer와 UI 연결
    // connect(player, &VideoStreamPlayer::frameReady, this, [&](const QImage &frame) {
    //     ui->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    // });

    // "설정" 버튼 클릭 시 슬롯 연결
    connect(ui->setting, &QPushButton::clicked, this, &MainWindow::onsettingsButtonclicked);

    connect(player, &VideoStreamPlayer::frameReady, this, [&](const QImage &frame) {
        // 비디오 스트림에서 받은 원본 프레임을 밝기, 대비, 채도 조정 함수에 전달하여 수정
        QImage adjustedFrame = frame; // 프레임을 그대로 복사하여 시작
        adjustedFrame = player->adjustBrightness(adjustedFrame, brightness);
        adjustedFrame = player->adjustContrast(adjustedFrame, contrast);
        adjustedFrame = player->adjustSaturation(adjustedFrame, saturation);

        // 수정된 프레임을 비디오 위젯에 표시
        ui->videoLabel->setPixmap(QPixmap::fromImage(adjustedFrame).scaled(ui->videoLabel->size(), Qt::KeepAspectRatio));
    });

    // SSL 인증서 설정
    sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setProtocol(QSsl::TlsV1_3);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);

    QFile certFile(":/certs/server.cert");
    if (!certFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open certificate file";
    } 
    else {
        QSslCertificate selfSignedCert(&certFile, QSsl::Pem);
        if (selfSignedCert.isNull()) {
            qDebug() << "Failed to parse certificate";
        }
        else {
            qDebug() << "Certificate loaded successfully";
            sslConfig.setCaCertificates({selfSignedCert});
            frameSSLSocket->setSslConfiguration(sslConfig);
            jsonSSLSocket->setSslConfiguration(sslConfig);
        }
    }

    // 비디오 SSL 소켓 연결
    frameSSLSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    frameSSLSocket->ignoreSslErrors();
    frameSSLSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(frameSSLSocket, &QSslSocket::encrypted, this, [&]() {
        qDebug() << "Frame SSL connection established.";
    });
    connect(frameSSLSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), 
                     [&](const QList<QSslError> &errors) {
        for (const auto &error : errors)
            qDebug() << "SSL Error:" << error.errorString();
        frameSSLSocket->ignoreSslErrors();
    });
    
    frameSSLSocket->connectToHostEncrypted("192.168.50.7", 12345);
    if (!frameSSLSocket->waitForEncrypted(3000)) {
        qDebug() << "Error:" << frameSSLSocket->errorString();
    }

    // 스트림 시작
    player->startStream(frameSSLSocket, 1280, 720, 1280 * 720 * 3);

    //json ssl 소켓 연결
    jsonSSLSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    jsonSSLSocket->ignoreSslErrors();
    jsonSSLSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(jsonSSLSocket, &QSslSocket::encrypted, this, [&]() {
        qDebug() << "JSON SSL connection established.";
    });
    connect(jsonSSLSocket, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), 
                     [&](const QList<QSslError> &errors) {
        for (const auto &error : errors)
            qDebug() << "SSL Error:" << error.errorString();
        jsonSSLSocket->ignoreSslErrors();
    });

    jsonSSLSocket->connectToHostEncrypted("192.168.50.7", 54321);
    if (!jsonSSLSocket->waitForEncrypted(3000)) {
        qDebug() << "Error:" << jsonSSLSocket->errorString();
    }
    connect(jsonSSLSocket, &QSslSocket::readyRead, this, &MainWindow::onJsonReadyRead);
    

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

    //데베
    eventLogManager = new EventLogManager("/Volumes/jjeongni/QtProgramming/JIKIMZON_Qt/test_gui/event_log.db", this);
    qDebug() << "EventLogManager 초기화 완료";

    player->setEventLogManager(eventLogManager);

    // QTableView에 사용할 모델 생성
    model = new QStandardItemModel(this);
    ui->tableView->setModel(model); // 테이블 뷰에 모델 설정
    qDebug() << "테이블 뷰 모델 설정 완료";

    ui->tableView->horizontalHeader()->setVisible(true);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setWordWrap(true);
    ui->tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    //검색 버튼 클릭 시 슬롯 연결
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
    qDebug() << "검색 버튼 시그널 연결 완료";

    // Dashboard 생성 및 추가
    dashboard = new DashboardWidget(this);

    if (ui->dashboardContainer->layout()) {
        ui->dashboardContainer->layout()->addWidget(dashboard);
    } else {
        QVBoxLayout *layout = new QVBoxLayout(ui->dashboardContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(dashboard);
        ui->dashboardContainer->setLayout(layout);
    }
    connect(player, &VideoStreamPlayer::dashobjectDetected,
            dashboard, &DashboardWidget::updateDetectedObjects);

    // 캘린더 생성 및 추가
    calender = new CalendarWidget(this);

    if (ui->calenderContainer->layout()) {
        ui->calenderContainer->layout()->addWidget(calender);
    } else {
        QVBoxLayout *layout = new QVBoxLayout(ui->calenderContainer);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(calender);
        ui->calenderContainer->setLayout(layout);
    }

    // 5. 각 버튼 클릭 시 화면 전환
    connect(ui->MButton, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(3);
    });

    connect(ui->RButton, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);  // 대시보드 화면으로 전환
    });

    connect(ui->CButton, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);  // 캘린더 화면으로 전환
    });

    connect(ui->EButton, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(2);  // 이벤트 로그 화면으로 전환
    });

    // if (!frameSSLSocket->waitForReadyRead(10000)) {
    //     qDebug() << "[mainwindow] SSL wait for ready read: " << frameSSLSocket->errorString();
    //     return;
    // }
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

    ui->tableView->setModel(model);
    qDebug() << "테이블 초기 데이터 로드 완료";
}

void MainWindow::toggleMode() {
    isNightMode = !isNightMode;

    // // opacity effect를 만들어서 아이콘을 변경하는 동안 부드러운 전환 효과를 줄 수 있습니다.
    // QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect();
    // ui->btnToggleMode->setGraphicsEffect(effect);

    // if (isNightMode) {
    //     setDarkMode();
    //     ui->btnToggleMode; // 밤 모드 아이콘
    // } else {
    //     setLightMode();
    //     ui->btnToggleMode; // 낮 모드 아이콘
    // }
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
    QString currentDate = koreanLocale.toString(now, "yyyy년 MM월 dd일 dddd"); // 한국어 날짜와 요일
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
    if (frameSSLSocket->state() == QAbstractSocket::ConnectedState) {
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
                                       "<span style='vertical-align:middle;'>"
                                       "<img src='data:image/png;base64,%1' width='32' height='32' style='vertical-align:middle;'>"
                                       " 날씨: %2°C, %3"
                                       "</span>"
                                       "</body></html>"
                                       ).arg(base64Data)
                                       .arg(roundedTemp)
                                       .arg(description);
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

void MainWindow::onsettingsButtonclicked()
{
    SliderDialog dialog(this);

    // 현재 밝기, 대비, 채도 값을 팝업 창에 전달
    dialog.setBrightness(brightness + 50);  // -50 ~ 50 값을 0 ~ 100으로 변환
    dialog.setContrast(contrast + 50);
    dialog.setSaturation(saturation);

    if (dialog.exec() == QDialog::Accepted) {
        // 팝업 창에서 값 가져오기
        brightness = dialog.getBrightness() - 50;  // 다시 -50 ~ 50 범위로 변환
        contrast = dialog.getContrast() - 50;
        saturation = dialog.getSaturation();

        qDebug() << "New Brightness:" << brightness;
        qDebug() << "New Contrast:" << contrast;
        qDebug() << "New Saturation:" << saturation;
    }
}

void MainWindow::onJsonReadyRead()
{
    QByteArray jsonData = jsonSSLSocket->readAll();

    // 디버깅: 수신된 JSON 데이터 출력
    qDebug() << "Received JSON:" << jsonData;

    // VideoStreamPlayer의 데이터 처리 함수 호출
    player->parseObjectDetectionData(jsonData);
}
