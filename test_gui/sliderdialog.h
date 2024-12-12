#ifndef SLIDERDIALOG_H
#define SLIDERDIALOG_H

#include <QWidget>
#include <QDialog>

namespace Ui {
class SliderDialog;
}

class SliderDialog : public QDialog // QDialog 상속
{
    Q_OBJECT

public:
    explicit SliderDialog(QWidget *parent = nullptr);
    ~SliderDialog();

    int getBrightness() const;
    int getContrast() const;
    int getSaturation() const;

    void setBrightness(int value);
    void setContrast(int value);
    void setSaturation(int value);


private:
    Ui::SliderDialog *ui;

    float brightness = 0.0f;
    float contrast = 1.0f;
    float saturation = 1.0f;
};

#endif // SLIDERDIALOG_H
