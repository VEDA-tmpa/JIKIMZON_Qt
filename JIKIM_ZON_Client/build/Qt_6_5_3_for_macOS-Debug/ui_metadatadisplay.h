/********************************************************************************
** Form generated from reading UI file 'metadatadisplay.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_METADATADISPLAY_H
#define UI_METADATADISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MetaDataDisplay
{
public:
    QGridLayout *gridLayout;
    QLabel *iconLabel;
    QLabel *timeLabel;
    QLabel *locationLabel;
    QLabel *typeLabel;
    QListWidget *eventLog;

    void setupUi(QWidget *MetaDataDisplay)
    {
        if (MetaDataDisplay->objectName().isEmpty())
            MetaDataDisplay->setObjectName("MetaDataDisplay");
        MetaDataDisplay->resize(790, 362);
        gridLayout = new QGridLayout(MetaDataDisplay);
        gridLayout->setObjectName("gridLayout");
        iconLabel = new QLabel(MetaDataDisplay);
        iconLabel->setObjectName("iconLabel");
        iconLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        color: #333333;                /* \355\205\215\354\212\244\355\212\270 \354\203\211\354\203\201 */\n"
"        background-color: #f0f0f0;     /* \353\260\260\352\262\275 \354\203\211 */\n"
"        border: 1px solid #ccc;        /* \355\205\214\353\221\220\353\246\254 */\n"
"        padding: 10px;                 /* \353\202\264\353\266\200 \354\227\254\353\260\261 */\n"
"        font-size: 16px;               /* \355\217\260\355\212\270 \355\201\254\352\270\260 */\n"
"        font-family: Arial, sans-serif;/* \355\217\260\355\212\270 \354\242\205\353\245\230 */\n"
"        font-weight: bold;             /* \355\217\260\355\212\270 \352\265\265\352\270\260 */\n"
"        text-align: center;            /* \355\205\215\354\212\244\355\212\270 \354\240\225\353\240\254 */\n"
"        border-radius: 5px;            /* \355\205\214\353\221\220\353\246\254 \353\221\245\352\270\200\352\270\260 */\n"
"    }\n"
"\n"
"    QLabel:hover {\n"
"        background-color: #e0e0e0;     /* \353\247\210\354\232"
                        "\260\354\212\244\353\245\274 \354\230\254\353\240\270\354\235\204 \353\225\214 \353\260\260\352\262\275\354\203\211 */\n"
"    }\n"
"\n"
"    QLabel:focus {\n"
"        border: 2px solid #4CAF50;     /* \355\217\254\354\273\244\354\212\244 \354\213\234 \355\205\214\353\221\220\353\246\254 \354\203\211\354\203\201 */\n"
"    }"));

        gridLayout->addWidget(iconLabel, 0, 0, 3, 1);

        timeLabel = new QLabel(MetaDataDisplay);
        timeLabel->setObjectName("timeLabel");
        timeLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        color: #333333;                /* \355\205\215\354\212\244\355\212\270 \354\203\211\354\203\201 */\n"
"        background-color: #f0f0f0;     /* \353\260\260\352\262\275 \354\203\211 */\n"
"        border: 1px solid #ccc;        /* \355\205\214\353\221\220\353\246\254 */\n"
"        padding: 10px;                 /* \353\202\264\353\266\200 \354\227\254\353\260\261 */\n"
"        font-size: 13px;               /* \355\217\260\355\212\270 \355\201\254\352\270\260 */\n"
"        font-family: Arial, sans-serif;/* \355\217\260\355\212\270 \354\242\205\353\245\230 */\n"
"        font-weight: bold;             /* \355\217\260\355\212\270 \352\265\265\352\270\260 */\n"
"        text-align: center;            /* \355\205\215\354\212\244\355\212\270 \354\240\225\353\240\254 */\n"
"        border-radius: 5px;            /* \355\205\214\353\221\220\353\246\254 \353\221\245\352\270\200\352\270\260 */\n"
"    }\n"
"\n"
"    QLabel:hover {\n"
"        background-color: #e0e0e0;     /* \353\247\210\354\232"
                        "\260\354\212\244\353\245\274 \354\230\254\353\240\270\354\235\204 \353\225\214 \353\260\260\352\262\275\354\203\211 */\n"
"    }\n"
"\n"
"    QLabel:focus {\n"
"        border: 2px solid #4CAF50;     /* \355\217\254\354\273\244\354\212\244 \354\213\234 \355\205\214\353\221\220\353\246\254 \354\203\211\354\203\201 */\n"
"    }"));

        gridLayout->addWidget(timeLabel, 0, 1, 1, 1);

        locationLabel = new QLabel(MetaDataDisplay);
        locationLabel->setObjectName("locationLabel");
        locationLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        color: #333333;                /* \355\205\215\354\212\244\355\212\270 \354\203\211\354\203\201 */\n"
"        background-color: #f0f0f0;     /* \353\260\260\352\262\275 \354\203\211 */\n"
"        border: 1px solid #ccc;        /* \355\205\214\353\221\220\353\246\254 */\n"
"        padding: 10px;                 /* \353\202\264\353\266\200 \354\227\254\353\260\261 */\n"
"        font-size: 13px;               /* \355\217\260\355\212\270 \355\201\254\352\270\260 */\n"
"        font-family: Arial, sans-serif;/* \355\217\260\355\212\270 \354\242\205\353\245\230 */\n"
"        font-weight: bold;             /* \355\217\260\355\212\270 \352\265\265\352\270\260 */\n"
"        text-align: center;            /* \355\205\215\354\212\244\355\212\270 \354\240\225\353\240\254 */\n"
"        border-radius: 5px;            /* \355\205\214\353\221\220\353\246\254 \353\221\245\352\270\200\352\270\260 */\n"
"    }\n"
"\n"
"    QLabel:hover {\n"
"        background-color: #e0e0e0;     /* \353\247\210\354\232"
                        "\260\354\212\244\353\245\274 \354\230\254\353\240\270\354\235\204 \353\225\214 \353\260\260\352\262\275\354\203\211 */\n"
"    }\n"
"\n"
"    QLabel:focus {\n"
"        border: 2px solid #4CAF50;     /* \355\217\254\354\273\244\354\212\244 \354\213\234 \355\205\214\353\221\220\353\246\254 \354\203\211\354\203\201 */\n"
"    }"));

        gridLayout->addWidget(locationLabel, 1, 1, 1, 1);

        typeLabel = new QLabel(MetaDataDisplay);
        typeLabel->setObjectName("typeLabel");
        typeLabel->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        color: #333333;                /* \355\205\215\354\212\244\355\212\270 \354\203\211\354\203\201 */\n"
"        background-color: #f0f0f0;     /* \353\260\260\352\262\275 \354\203\211 */\n"
"        border: 1px solid #ccc;        /* \355\205\214\353\221\220\353\246\254 */\n"
"        padding: 10px;                 /* \353\202\264\353\266\200 \354\227\254\353\260\261 */\n"
"        font-size: 13px;               /* \355\217\260\355\212\270 \355\201\254\352\270\260 */\n"
"        font-family: Arial, sans-serif;/* \355\217\260\355\212\270 \354\242\205\353\245\230 */\n"
"        font-weight: bold;             /* \355\217\260\355\212\270 \352\265\265\352\270\260 */\n"
"        text-align: center;            /* \355\205\215\354\212\244\355\212\270 \354\240\225\353\240\254 */\n"
"        border-radius: 5px;            /* \355\205\214\353\221\220\353\246\254 \353\221\245\352\270\200\352\270\260 */\n"
"    }\n"
"\n"
"    QLabel:hover {\n"
"        background-color: #e0e0e0;     /* \353\247\210\354\232"
                        "\260\354\212\244\353\245\274 \354\230\254\353\240\270\354\235\204 \353\225\214 \353\260\260\352\262\275\354\203\211 */\n"
"    }\n"
"\n"
"    QLabel:focus {\n"
"        border: 2px solid #4CAF50;     /* \355\217\254\354\273\244\354\212\244 \354\213\234 \355\205\214\353\221\220\353\246\254 \354\203\211\354\203\201 */\n"
"    }"));

        gridLayout->addWidget(typeLabel, 2, 1, 1, 1);

        eventLog = new QListWidget(MetaDataDisplay);
        eventLog->setObjectName("eventLog");
        eventLog->setStyleSheet(QString::fromUtf8("QListWidget {\n"
"        background-color: #f0f0f0;  /* \353\260\260\352\262\275\354\203\211 */\n"
"        border: 1px solid #ccc;     /* \355\205\214\353\221\220\353\246\254 */\n"
"        padding: 5px;               /* \354\227\254\353\260\261 */\n"
"        font-size: 14px;            /* \355\217\260\355\212\270 \355\201\254\352\270\260 */\n"
"    }\n"
"\n"
"    QListWidget::item {\n"
"        padding: 5px;\n"
"        background-color: transparent;  /* \355\225\255\353\252\251 \353\260\260\352\262\275 */\n"
"        border-bottom: 1px solid #e0e0e0;  /* \355\225\255\353\252\251 \354\202\254\354\235\264\354\227\220 \352\262\275\352\263\204\354\204\240 */\n"
"    }\n"
"\n"
"    QListWidget::item:hover {\n"
"        background-color: #d3d3d3;  /* \353\247\210\354\232\260\354\212\244 \355\230\270\353\262\204 \354\213\234 \353\260\260\352\262\275\354\203\211 */\n"
"    }\n"
"\n"
"    QListWidget::item:selected {\n"
"        background-color: #4CAF50;  /* \354\204\240\355\203\235\353\220\234 \355\225\255\353\252"
                        "\251 \353\260\260\352\262\275\354\203\211 */\n"
"        color: white;               /* \354\204\240\355\203\235\353\220\234 \355\225\255\353\252\251 \355\205\215\354\212\244\355\212\270 \354\203\211 */\n"
"    }\n"
"\n"
"    QListWidget::item:selected:!active {\n"
"        background-color: #3e8e41;  /* \354\204\240\355\203\235\353\220\234 \355\225\255\353\252\251\354\235\264 \353\271\204\355\231\234\354\204\261\354\235\274 \353\225\214 \354\203\211\354\203\201 */\n"
"    }"));

        gridLayout->addWidget(eventLog, 3, 0, 1, 2);


        retranslateUi(MetaDataDisplay);

        QMetaObject::connectSlotsByName(MetaDataDisplay);
    } // setupUi

    void retranslateUi(QWidget *MetaDataDisplay)
    {
        MetaDataDisplay->setWindowTitle(QCoreApplication::translate("MetaDataDisplay", "Form", nullptr));
        iconLabel->setText(QString());
        timeLabel->setText(QCoreApplication::translate("MetaDataDisplay", "\354\213\234\352\260\204: ", nullptr));
        locationLabel->setText(QCoreApplication::translate("MetaDataDisplay", "\354\234\204\354\271\230: ", nullptr));
        typeLabel->setText(QCoreApplication::translate("MetaDataDisplay", "\352\260\235\354\262\264 \354\242\205\353\245\230: ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MetaDataDisplay: public Ui_MetaDataDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_METADATADISPLAY_H
