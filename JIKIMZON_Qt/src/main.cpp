#include "mainwindow.h"
#include "decryptor.h"

#include <QApplication>
#include <QByteArray>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();

    // QString timestamp = "20241204_123456.789";
    // QFile efile(":/res/res/encrypted.bin");
    // if (!efile.open(QIODevice::ReadOnly))
    // {
    //     qDebug() << "Failed to open file for reading: encrypted.bin";
    //     return -1;
    // }

    // QFile ofile(":/res/res/origin.bin");
    // if (!ofile.open(QIODevice::ReadOnly))
    // {
    //     qDebug() << "Failed to open file for reading: origin.bin";
    //     return -1;
    // }

    // QByteArray originData = ofile.readAll();
    // QByteArray encryptedData = efile.readAll();
    // QByteArray decryptedData;

    // Decryptor decryptor;
    // decryptor.Decrypt(timestamp, encryptedData, decryptedData);

    // qDebug() << "Decrypted data size: " << decryptedData.size();

    // QFile dfile("C://Users//sihyu//OneDrive - Kumoh//Source_File//QtCreator//VedaFinal//JIKIMZON_Qt//JIKIMZON_Qt//res//decrypted.bin");
    // if (!dfile.open(QIODevice::WriteOnly))
    // {
    //     qDebug() << "Failed to open file for writing: decrypted.bin";
    //     return -1;
    // }
    // dfile.write(decryptedData);

    // qDebug() << "Decryption done";

    // efile.close();
    // dfile.close();

    // for (int i = 0; i < originData.size(); i++)
    // {
    //     //qDebug() << originData[i];
    //     std::cout << static_cast<int>(originData[i]) << std::endl;
    // }


    QTcpSocket socket;
    socket.connectToHost("192.168.50.14", 12345);

    if (!socket.waitForConnected(30000))
    {
        qDebug() << "Error: " << socket.errorString();
        return -1;
    }

    if (socket.state() == QAbstractSocket::ConnectedState) {
        qDebug() << "Connected to server!";
    }
    else
    {
        qDebug() << "Failed to connect to server. Current state:" << socket.state();
    }

    std::vector<uint8_t> buffer;
    buffer.reserve(100);
    buffer.resize(100);
    
    int readSize = 0;
    while (readSize < 100)
    {
        socket.waitForReadyRead(10000);
        int read = socket.read(reinterpret_cast<char*>(buffer.data() + readSize), 100 - readSize);
        if (read == -1)
        {
            qDebug() << "Error: " << socket.errorString();
            return -1;
        }

        readSize += read;
    }

    qDebug() << "ReadAllData(vector) - readSize: " << readSize;

    Decryptor decryptor;
    std::vector<uint8_t> decryptedData;

    QString dump;
    decryptor.Decrypt(dump, buffer, decryptedData);

    for (const auto& data : buffer)
    {
        std::cout << static_cast<int>(data) << " ";
    }
    std::cout << "\n=======\n" << std::endl;
    for (const auto& data : decryptedData)
    {
        std::cout << static_cast<int>(data) << " ";
    }


    exit(0);

    return a.exec();
}
