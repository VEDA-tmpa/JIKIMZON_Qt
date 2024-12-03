#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUI(new Ui::MainWindow)
{
    mUI->setupUi(this);

    // init player
    mVideoStreamPlayer = new VideoStreamPlayer();
    mVideoStreamPlayer->InitStreamPlayer("192.168.35.221", 1234, 4321, 1280, 720, 100000, 15);

    // init meta data display
    MetaDataDisplay* metaData = new MetaDataDisplay(this);

    // metaDataContainer에 MetaDataDisplay 추가
    if (mUI->metaDataContainer->layout())
    {
        mUI->metaDataContainer->layout()->addWidget(metaData);
    }
    else
    {
        // 레이아웃이 없는 경우 새로 설정
        QVBoxLayout* layout = new QVBoxLayout(mUI->metaDataContainer);
        layout->setContentsMargins(0,0,0,0);
        layout->addWidget(metaData);
        mUI->metaDataContainer->setLayout(layout);
    }

        // example data
        // metaData->updateMetaData("2024-11-15 10:20", "A구역", "paper");

    // event log
    // QString dbPath = QDir::currentPath() + "/res/event_log.db";
    // mEventLogManager = new EventLogManager(dbPath, this);

    // QTableView에 사용할 모델 생성
    mItemModel = new QStandardItemModel(this);
    mUI->eventlogtableView->setModel(mItemModel);


    // connect theme button
    connect(mUI->btnToggleMode, &QPushButton::clicked, this, &MainWindow::toggleMode);
    mUI->btnToggleMode->setIcon(QIcon(":/icon/sun.png"));
    mUI->btnToggleMode->setIconSize(QSize(20, 20));

    // connect player
    connect(mVideoStreamPlayer, &VideoStreamPlayer::FrameReady, this, [&](const QImage& frame) {
        qDebug() << "[MainWindow] FrameReady signal received";
        mUI->videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(mUI->videoLabel->size(), Qt::KeepAspectRatio));
    });

    // connect video stream buttons
    connect(mUI->pauseButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::PauseStream);
    connect(mUI->resumeButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::ResumeStream);
    connect(mUI->backwardButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::GoBackward);
    connect(mUI->forwardButton, &QPushButton::clicked, mVideoStreamPlayer, &VideoStreamPlayer::GoForward);

    // connect search button
    connect(mUI->searchButton, &QPushButton::clicked, this, &MainWindow::onsearchButtonclicked);

    // connect sliders
    mUI->brightnessSlider->setValue(0);
    mUI->brightnessSlider->setRange(-255, 255);
    mUI->saturationSlider->setValue(0);
    mUI->saturationSlider->setRange(-100, 100);
    mUI->sharpnessSlider->setValue(0);
    mUI->sharpnessSlider->setRange(-100, 100);

    connect(mUI->brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onBrightnessSliderChanged);
    connect(mUI->saturationSlider, &QSlider::valueChanged, this, &MainWindow::onSaturationSliderChanged);
    connect(mUI->sharpnessSlider, &QSlider::valueChanged, this, &MainWindow::onSharpnessSliderChanged);

}

MainWindow::~MainWindow()
{
    mVideoStreamPlayer->StopStream();
    delete mUI;
}

void MainWindow::onsearchButtonclicked() {
    qDebug() << "on_searchButton_clicked 호출";

    QString searchTerm = mUI->eventlineEdit->text();            // QLineEdit에서 검색어 가져오기
    QString selectedValue = mUI->eventcomboBox->currentText(); // 콤보박스에서 선택된 값 가져오기

    qDebug() << "[MainWindow onSearchButtonClicked] 검색어: " << searchTerm << ", 선택값: " << selectedValue;

    mItemModel->clear(); // 이전 데이터 지우기
    mItemModel->setHorizontalHeaderLabels({"ID", "Frame ID", "Timestamp", "Object Class", "X", "Y", "Width", "Height"}); // 헤더 설정

    // 데이터베이스에서 해당 값을 검색
    QString query = QString("SELECT * FROM event_logs WHERE object_class LIKE '%%1%' AND object_class = '%2'")
                        .arg(searchTerm)
                        .arg(selectedValue);

    QSqlQuery sqlQuery(query);

    if (sqlQuery.exec()) {
        qDebug() << "SQL Query 실행 성공";
        while (sqlQuery.next()) {
            QList<QStandardItem*> rowItems;
            for (int i = 0; i < sqlQuery.record().count(); ++i) {
                rowItems.append(new QStandardItem(sqlQuery.value(i).toString()));
            }
            mItemModel->appendRow(rowItems); // 모델에 행 추가
        }
        qDebug() << "검색 결과 처리 완료";
    } else {
        qDebug() << "SQL Query 실행 실패: " << sqlQuery.lastError().text();
        QMessageBox::warning(this, "Error", "Failed to execute query: " + sqlQuery.lastError().text());
    }
}

// slots
void MainWindow::toggleMode()
{
    mbNightMode = !mbNightMode;

    if (mbNightMode)
    {
        setDarkMode();
        mUI->btnToggleMode->setIcon(QIcon(":/icon/sun.png")); // 밤 모드 아이콘
    }
    else
    {
        setLightMode();
        mUI->btnToggleMode->setIcon(QIcon(":/icon/moon.png")); // 낮 모드 아이콘
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

void MainWindow::onBrightnessSliderChanged(int value) {
    if (!mCurrentFrame.isNull()) {
        QImage adjustedImage = mCurrentFrame.copy();
        applyBrightnessEffect(adjustedImage, value); // 밝기 효과 적용

    }
}

void MainWindow::onSaturationSliderChanged(int value) {
    if (!mCurrentFrame.isNull()) {
        QImage adjustedImage = mCurrentFrame.copy();
        applySaturationEffect(adjustedImage, value); // 채도 효과 적용

    }
}

void MainWindow::onSharpnessSliderChanged(int value) {
    if (!mCurrentFrame.isNull()) {
        QImage adjustedImage = mCurrentFrame.copy();
        applySharpnessEffect(adjustedImage, value); // 선명도 효과 적용

    }
}

void MainWindow::applyBrightnessEffect(QImage &image, int brightness) {
    // 밝기 조정 로직 (예: -255 ~ 255)
    if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
        image = image.convertToFormat(QImage::Format_RGB32);
    }

    uchar *bits = image.bits();
    int bytesPerLine = image.bytesPerLine();
    int height = image.height();

    for (int y = 0; y < height; ++y) {
        uchar *line = bits + y * bytesPerLine;
        for (int x = 0; x < image.width(); ++x) {
            uchar *pixel = line + x * 4; // Assuming RGB32 or ARGB32
            pixel[0] = qBound(0, pixel[0] + brightness, 255); // Blue
            pixel[1] = qBound(0, pixel[1] + brightness, 255); // Green
            pixel[2] = qBound(0, pixel[2] + brightness, 255); // Red
        }
    }
}

void MainWindow::applySaturationEffect(QImage &image, int saturation) {
    // 채도 값의 범위를 -100 ~ 100으로 설정
    float factor = (saturation + 100) / 100.0f; // 0.0 ~ 2.0 범위로 변환

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor pixelColor = image.pixelColor(x, y);
            // RGB 값을 [0, 1] 범위로 정규화
            float r = pixelColor.red() / 255.0f;
            float g = pixelColor.green() / 255.0f;
            float b = pixelColor.blue() / 255.0f;

            // RGB를 HSL로 변환
            float max = qMax(r, qMax(g, b));
            float min = qMin(r, qMin(g, b));
            float h, s, l = (max + min) / 2.0f;

            if (max == min) {
                h = s = 0; // achromatic
            } else {
                float d = max - min;
                s = l > 0.5f ? d / (2 - max - min) : d / (max + min);
                if (max == r) {
                    h = (g - b) / d + (g < b ? 6 : 0);
                } else if (max == g) {
                    h = (b - r) / d + 2;
                } else {
                    h = (r - g) / d + 4;
                }
                h /= 6;
            }

            // 채도 조정
            s *= factor;
            s = qBound(0.0f, s, 1.0f); // 채도는 [0,1] 범위로 제한

            // HSL을 RGB로 다시 변환
            if (s == 0) {
                r = g = b = l; // achromatic
            } else {
                auto hueToRgb = [](float p, float q, float t) {
                    if (t < 0) t += 1;
                    if (t > 1) t -= 1;
                    if (t < 1/6.0f) return p + (q - p) * 6 * t;
                    if (t < 1/2.0f) return q;
                    if (t < 2/3.0f) return p + (q - p) * (2/3.0f - t) * 6;
                    return p;
                };

                float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
                float p = 2 * l - q;

                r = hueToRgb(p, q, h + 1/3.0f);
                g = hueToRgb(p, q, h);
                b = hueToRgb(p, q, h - 1/3.0f);
            }

            // RGB 값을 [0,255] 범위로 변환하여 픽셀 색상 업데이트
            pixelColor.setRed(qBound(0, static_cast<int>(r * 255), 255));
            pixelColor.setGreen(qBound(0, static_cast<int>(g * 255), 255));
            pixelColor.setBlue(qBound(0, static_cast<int>(b * 255), 255));

            image.setPixelColor(x, y, pixelColor);
        }
    }
}

void MainWindow::applySharpnessEffect(QImage &image, int sharpness) {
    // 선명도 값의 범위를 -100 ~ 100으로 설정
    float factor = sharpness / 100.0f; // -1 ~ 1 범위로 변환

    if (factor == 0) return; // 선명도 변경이 없을 경우

    QImage result(image.size(), QImage::Format_RGB32);

    for (int y = 1; y < image.height() - 1; ++y) {
        for (int x = 1; x < image.width() - 1; ++x) {
            QColor colorCenter = image.pixelColor(x, y);
            QColor colorLeft   = image.pixelColor(x - 1, y);
            QColor colorRight  = image.pixelColor(x + 1, y);
            QColor colorTop    = image.pixelColor(x, y - 1);
            QColor colorBottom = image.pixelColor(x, y + 1);

            // 평균 RGB 값 계산
            int avgR = (colorLeft.red() + colorRight.red() + colorTop.red() + colorBottom.red()) / 4;
            int avgG = (colorLeft.green() + colorRight.green() + colorTop.green() + colorBottom.green()) / 4;
            int avgB = (colorLeft.blue() + colorRight.blue() + colorTop.blue() + colorBottom.blue()) / 4;

            // 샤프닝 적용
            int newR = qBound(0, static_cast<int>(colorCenter.red() + factor * (colorCenter.red() - avgR)), 255);
            int newG = qBound(0, static_cast<int>(colorCenter.green() + factor * (colorCenter.green() - avgG)), 255);
            int newB = qBound(0, static_cast<int>(colorCenter.blue() + factor * (colorCenter.blue() - avgB)), 255);

            result.setPixelColor(x, y, QColor(newR, newG, newB));
        }
    }

    // 결과 이미지를 기존 이미지에 복사
    image.swap(result);
}
