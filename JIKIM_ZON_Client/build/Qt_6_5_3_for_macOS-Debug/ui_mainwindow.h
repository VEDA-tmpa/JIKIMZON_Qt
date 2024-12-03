/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnToggleMode;
    QLabel *videoLabel;
    QWidget *metaDataContainer;
    QTabWidget *tabWidget;
    QWidget *event;
    QComboBox *eventcomboBox;
    QLineEdit *eventlineEdit;
    QPushButton *searchButton;
    QTableView *eventlogtableView;
    QWidget *tab_2;
    QSlider *brightnessSlider;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QSlider *saturationSlider;
    QSlider *sharpnessSlider;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1118, 702);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnToggleMode = new QPushButton(centralwidget);
        btnToggleMode->setObjectName("btnToggleMode");
        btnToggleMode->setGeometry(QRect(940, 10, 51, 51));
        btnToggleMode->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: rgb(229, 231, 158); /* \353\260\260\352\262\275\354\203\211 */\n"
"    border: 2px solid rgb(69, 61, 53); /* \355\205\214\353\221\220\353\246\254 \354\203\211\354\203\201\352\263\274 \353\221\220\352\273\230 */\n"
"    border-radius: 25px; /* \353\262\204\355\212\274 \355\201\254\352\270\260\354\235\230 \354\240\210\353\260\230\354\234\274\353\241\234 \354\204\244\354\240\225 (\354\240\225\355\231\225\355\225\234 \354\233\220\355\230\225) */\n"
"    color: white; /* \355\205\215\354\212\244\355\212\270 \354\203\211\354\203\201 */\n"
"    font-size: 15px; /* \355\205\215\354\212\244\355\212\270 \355\201\254\352\270\260 */\n"
"    width: 30px; /* \353\262\204\355\212\274\354\235\230 \353\204\210\353\271\204 */\n"
"    height: 30px; /* \353\262\204\355\212\274\354\235\230 \353\206\222\354\235\264 */\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background-color: rgb(170, 197, 227); /* \355\230\270\353\262\204 \354\203\201\355\203\234 \353\260\260\352\262\275\354\203\211 */\n"
""
                        "}\n"
"\n"
"QPushButton:pressed {\n"
"    background-color: rgb(252, 253, 248); /* \355\201\264\353\246\255 \354\213\234 \353\260\260\352\262\275\354\203\211 */\n"
"}"));
        videoLabel = new QLabel(centralwidget);
        videoLabel->setObjectName("videoLabel");
        videoLabel->setGeometry(QRect(30, 20, 551, 381));
        metaDataContainer = new QWidget(centralwidget);
        metaDataContainer->setObjectName("metaDataContainer");
        metaDataContainer->setGeometry(QRect(610, 80, 491, 321));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(600, 430, 501, 211));
        event = new QWidget();
        event->setObjectName("event");
        eventcomboBox = new QComboBox(event);
        eventcomboBox->setObjectName("eventcomboBox");
        eventcomboBox->setGeometry(QRect(10, 20, 103, 32));
        eventlineEdit = new QLineEdit(event);
        eventlineEdit->setObjectName("eventlineEdit");
        eventlineEdit->setGeometry(QRect(110, 20, 291, 31));
        searchButton = new QPushButton(event);
        searchButton->setObjectName("searchButton");
        searchButton->setGeometry(QRect(410, 20, 81, 32));
        eventlogtableView = new QTableView(event);
        eventlogtableView->setObjectName("eventlogtableView");
        eventlogtableView->setGeometry(QRect(20, 60, 461, 101));
        tabWidget->addTab(event, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        brightnessSlider = new QSlider(tab_2);
        brightnessSlider->setObjectName("brightnessSlider");
        brightnessSlider->setGeometry(QRect(120, 30, 160, 25));
        brightnessSlider->setOrientation(Qt::Horizontal);
        label = new QLabel(tab_2);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 30, 58, 16));
        label_2 = new QLabel(tab_2);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(30, 60, 58, 16));
        label_3 = new QLabel(tab_2);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(30, 90, 58, 16));
        saturationSlider = new QSlider(tab_2);
        saturationSlider->setObjectName("saturationSlider");
        saturationSlider->setGeometry(QRect(120, 60, 160, 25));
        saturationSlider->setOrientation(Qt::Horizontal);
        sharpnessSlider = new QSlider(tab_2);
        sharpnessSlider->setObjectName("sharpnessSlider");
        sharpnessSlider->setGeometry(QRect(120, 90, 160, 25));
        sharpnessSlider->setOrientation(Qt::Horizontal);
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1118, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnToggleMode->setText(QString());
        videoLabel->setText(QString());
        searchButton->setText(QCoreApplication::translate("MainWindow", "\352\262\200\354\203\211", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(event), QCoreApplication::translate("MainWindow", "\354\235\264\353\262\244\355\212\270", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\353\260\235\352\270\260", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\354\261\204\353\217\204", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\354\204\240\353\252\205\353\217\204", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\353\271\204\353\224\224\354\230\244\354\204\244\354\240\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
