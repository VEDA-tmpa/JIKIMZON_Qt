#include "sliderdialog.h"
#include "ui_sliderdialog.h"
#include <QDialog>

SliderDialog::SliderDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SliderDialog)
{
    ui->setupUi(this);

    // 초기 슬라이더 값 설정
    ui->brightnessSlider->setMinimum(0);
    ui->brightnessSlider->setMaximum(100);
    ui->brightnessSlider->setValue(50); // 초기 값

    ui->contrastSlider->setMinimum(0);
    ui->contrastSlider->setMaximum(100);
    ui->contrastSlider->setValue(50);

    ui->saturationSlider->setMinimum(0);
    ui->saturationSlider->setMaximum(100);
    ui->saturationSlider->setValue(0);

    // 버튼 연결
    // 확인 버튼 클릭 시 다이얼로그 종료
    connect(ui->okButton, &QPushButton::clicked, this, &SliderDialog::accept);
    // 취소 버튼 클릭 시 다이얼로그 종료
    connect(ui->cancelButton, &QPushButton::clicked, this, &SliderDialog::reject);
}

SliderDialog::~SliderDialog()
{
    delete ui;
}

// Getter 함수
int SliderDialog::getBrightness() const { return ui->brightnessSlider->value(); }
int SliderDialog::getContrast() const { return ui->contrastSlider->value(); }
int SliderDialog::getSaturation() const { return ui->saturationSlider->value(); }

// Setter 함수
void SliderDialog::setBrightness(int value) { ui->brightnessSlider->setValue(value); }
void SliderDialog::setContrast(int value) { ui->contrastSlider->setValue(value); }
void SliderDialog::setSaturation(int value) { ui->saturationSlider->setValue(value); }
