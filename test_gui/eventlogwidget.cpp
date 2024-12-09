#include "eventlogwidget.h"
#include "ui_eventlogwidget.h"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QSqlRecord>

EventLogWidget::EventLogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventLogWidget)
{
    ui->setupUi(this);

    // 데이터베이스 연결 초기화
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("/Volumes/jjeongni/QtProgramming/JIKIMZON_Qt/test_gui/event_log.db");

    if (!db.open()) {
        qDebug() << "Error: " << db.lastError().text();
        return;
    }

    // QTableView에 사용할 모델 생성
    model = new QStandardItemModel(this);
    ui->eventlogtableView->setModel(model); // 테이블 뷰에 모델 설정
    qDebug() << "테이블 뷰 모델 설정 완료";

    //검색 버튼 클릭 시 슬롯 연결
    connect(ui->searchButton, &QPushButton::clicked, this, &EventLogWidget::onsearchButtonclicked);
    qDebug() << "검색 버튼 시그널 연결 완료";
}

EventLogWidget::~EventLogWidget()
{
    delete ui;
}


void EventLogWidget::onsearchButtonclicked() {
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

void EventLogWidget::loadEventLogs() {
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

