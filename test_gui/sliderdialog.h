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
};

#endif // SLIDERDIALOG_H
