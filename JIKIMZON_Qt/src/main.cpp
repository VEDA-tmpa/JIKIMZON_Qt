#include "mainwindow.h"
#include "decryptor.h"

#include <QApplication>
#include <QByteArray>

#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 12345
#define BUF_SIZE 1024

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

    /////////////////
    /// \brief udpSocket

    qDebug() << "start udp";

    int sockfd;
    struct sockaddr_in serverAddr;
    std::vector<uint8_t> buffer(100);

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Fill server address structure
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int n = recvfrom(sockfd, buffer.data(), 100, MSG_WAITALL, nullptr, nullptr);

    Decryptor decryptor;
    std::vector<uint8_t> decryptedData;
    std::vector<uint8_t> encryptedData;

    decryptor.Decrypt("20241204_123456.789", buffer, decryptedData);
    decryptor.EncryptData("20241204_123456.789", decryptedData, decryptedData.size(), encryptedData);

    for (int i = 0; i < n; ++i) {
        std::cout << static_cast<int>(buffer[i]) << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < decryptedData.size(); ++i) {
        std::cout << static_cast<int>(decryptedData[i]) << " ";
    }
    std::cout << std::endl;
    for (int i = 0; i < encryptedData.size(); ++i) {
        std::cout << static_cast<int>(encryptedData[i]) << " ";
    }

    // Close the socket
    close(sockfd);



    // QUdpSocket udpSocket;

    // quint16 port = 12345; // 포트 번호
    // if (!udpSocket.bind(QHostAddress::Any, port)) {
    //     qDebug() << "Failed to bind socket:" << udpSocket.errorString();
    //     return 1;
    // }

    // qDebug() << "Listening on port" << port;

    // Decryptor decryptor;
    // QString str;
    // std::string sstr;

    // QObject::connect(&udpSocket, &QUdpSocket::readyRead, [&]() {
    //     qDebug() << "readyRead";
        
    //     while (udpSocket.hasPendingDatagrams()) {
    //         std::vector<uint8_t> buffer;
    //         buffer.resize(udpSocket.pendingDatagramSize());
    //         QHostAddress senderAddress;
    //         quint16 senderPort;

    //         udpSocket.readDatagram(reinterpret_cast<char *>(buffer.data()), buffer.size(), &senderAddress, &senderPort);

    //         std::cout << "Received data from: " << senderAddress.toString().toStdString() << ":" << senderPort << std::endl;
            
    //         std::vector<uint8_t> decryptedData;
    //         std::vector<uint8_t> encryptedData;
    //         std::vector<uint8_t> decryptedData2;

    //         decryptor.Decrypt(str, buffer, decryptedData);
    //         decryptor.EncryptData(sstr, decryptedData, decryptedData.size(), encryptedData);
    //         decryptor.Decrypt(str, encryptedData, decryptedData2);

    //         for (const auto& data : buffer)
    //         {
    //             std::cout << +data << " ";
    //         }
    //         std::cout << "\n=======\n" << std::endl;
    //         for (const auto& data : decryptedData)
    //         {
    //             std::cout << +data << " ";
    //         }
    //         std::cout << "\n=======\n" << std::endl;
    //         for (const auto& data : encryptedData)
    //         {
    //             std::cout << +data << " ";
    //         }
    //         std::cout << "\n=======\n" << std::endl;
    //         for (const auto& data : decryptedData2)
    //         {
    //             std::cout << +data << " ";
    //         }
    //         std::cout << "\n=======\n" << std::endl;
    //     }
    // });


    // QTcpSocket socket;
    // socket.connectToHost("192.168.50.14", 12345);

    // if (!socket.waitForConnected(30000))
    // {
    //     qDebug() << "Error: " << socket.errorString();
    //     return -1;
    // }

    // if (socket.state() == QAbstractSocket::ConnectedState)
    // {
    //     qDebug() << "Connected to server!";
    // }
    // else
    // {
    //     qDebug() << "Failed to connect to server. Current state:" << socket.state();
    // }

    // int size = 100;
    // std::vector<uint8_t> buffer;
    // buffer.reserve(size);
    // // buffer.resize(size);
    
    // int readSize = 0;
    // while (readSize < size)
    // {
    //     socket.waitForReadyRead(10000);
    //     int read = socket.read(reinterpret_cast<char*>(buffer.data() + readSize), size - readSize);
    //     if (read == -1)
    //     {
    //         qDebug() << "Error: " << socket.errorString();
    //         return -1;
    //     }

    //     readSize += read;
    // }

    // qDebug() << "ReadAllData(vector) - readSize: " << readSize;

    // Decryptor decryptor;
    // std::vector<uint8_t> decryptedData;

    // QString dump;
    // decryptor.Decrypt(dump, buffer, decryptedData);

    // std::vector<uint8_t> aaa;
    // std::string dumps = "20241204_123456.789";
    // decryptor.EncryptData(dumps, decryptedData, decryptedData.size(), aaa);

    // for (const auto& data : buffer)
    // {
    //     std::cout << std::to_string(data) << " ";
    // }
    // std::cout << "\n=======\n" << std::endl;
    // for (const auto& data : decryptedData)
    // {
    //     std::cout << std::to_string(data) << " ";
    // }
    // std::cout << "\n=======\n" << std::endl;
    // for (const auto& data : aaa)
    // {
    //     std::cout << std::to_string(data) << " ";
    // }


    // exit(0);

    return a.exec();
}
