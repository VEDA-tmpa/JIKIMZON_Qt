#include "mainwindow.h"
#include "decryptor.h"

#include <QApplication>
#include <QByteArray>

#include <iostream>
#include <cstring>
// #include <arpa/inet.h>
// #include <sys/socket.h>
// #include <unistd.h>

#define PORT 12345
#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* w = new MainWindow();
    w->show();
    w->InitMainWindow();

    return a.exec();
}
