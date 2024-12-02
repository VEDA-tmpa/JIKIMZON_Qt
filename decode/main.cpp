#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.InitStreamPlayer("localhost", 12345, 1280, 720, 1000000, 15);
    w.RunStreamPlayer();
    
    return a.exec();
}
