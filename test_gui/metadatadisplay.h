#ifndef METADATADISPLAY_H
#define METADATADISPLAY_H

#include <QWidget>

namespace Ui {
class MetaDataDisplay;
}

class MetaDataDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit MetaDataDisplay(QWidget *parent = nullptr);
    ~MetaDataDisplay();

    void updateMetaData(const QString& time, const QString& location, const QString& objectType);

private:
    Ui::MetaDataDisplay *ui;
};

#endif // METADATADISPLAY_H
