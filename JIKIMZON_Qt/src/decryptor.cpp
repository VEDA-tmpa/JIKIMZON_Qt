#include "decryptor.h"

#include <QFile>
#include <QDir>
#include <QDebug>

#include <iostream>
#include <fstream>
#include <cstring>

Decryptor::Decryptor()
{
    mCtx = EVP_CIPHER_CTX_new();
    if (!mCtx) {
        qDebug() << "EVP_CIPHER_CTX_new failed!";
    }
    
    QString codePath = __FILE__;
    QString filepath = "C:/Users/sihyu/OneDrive - Kumoh/Source_File/QtCreator/VedaFinal/JIKIMZON_Qt/JIKIMZON_Qt/res/keyfile2.bin";
    LoadKey(filepath);
}

Decryptor::~Decryptor()
{
    if (mCtx)
    {
        EVP_CIPHER_CTX_free(mCtx);
    }
}


void Decryptor::EncryptData(std::string& timestamp, std::vector<uint8_t>& src, int size, std::vector<uint8_t>& OUT dest)
{   
    dest.clear();
    dest.resize(size);

    // unsigned char iv[12];
    // std::memcpy(iv, reinterpret_cast<const unsigned char*>(timestamp.substr(timestamp.length() - 12, 12).c_str()), 12);
    
    // std::cout << "timestamp: " << timestamp << std::endl;
    // std::cout << "iv: " << iv << std::endl;

    unsigned char iv[12];
    std::memset(iv, 0, 12);

    if (EVP_EncryptInit_ex(mCtx, EVP_chacha20(), nullptr, mKey, iv) != 1)
    {
        std::cerr << "Error: encrypt init" << std::endl;
    }

    int len;
    if (EVP_EncryptUpdate(mCtx, dest.data(), &len, src.data(), size) != 1)
    {
        std::cerr << "Error: encrypt update" << std::endl;
    }
}


void Decryptor::Decrypt(QString& nounceStr, const QByteArray& encryptedData, OUT QByteArray& decryptedData)
{   
    decryptedData.clear();
    decryptedData.resize(encryptedData.size());

    unsigned char iv[12];
    std::memset(iv, 0, 12);
    //std::memcpy(iv, reinterpret_cast<const unsigned char*>(nounceStr.right(12).toStdString().c_str()), 12);

    if (EVP_DecryptInit_ex(mCtx, EVP_chacha20(), nullptr,
                        reinterpret_cast<const unsigned char*>(mKey),
                        iv) != 1)
    {
        qDebug() << "EVP_DecryptInit_ex failed!";
        return;
    }

    int outLen;
    int totalLen = 0;
    if (EVP_DecryptUpdate(mCtx, reinterpret_cast<unsigned char*>(decryptedData.data()), &outLen,
                        reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size()) != 1)
    {
        qDebug() << "EVP_DecryptUpdate failed!";
        return;
    }

    totalLen += outLen;

    if (EVP_DecryptFinal_ex(mCtx, reinterpret_cast<unsigned char*>(decryptedData.data()) + totalLen, &outLen) != 1)
    {
        qDebug() << "EVP_DecryptFinal_ex failed!";
        return;
    }

    totalLen += outLen;
    decryptedData.resize(totalLen);
}

void Decryptor::Decrypt(QString& nounce, std::vector<uint8_t>& encryptedData, OUT std::vector<uint8_t>& decryptedData)
{   
    decryptedData.clear();
    decryptedData.resize(encryptedData.size());

    unsigned char iv[12];
    std::memset(iv, 0, 12);
    //std::memcpy(iv, reinterpret_cast<const unsigned char*>(nounceStr.right(12).toStdString().c_str()), 12);

    if (EVP_DecryptInit_ex(mCtx, EVP_chacha20(), nullptr,
                        reinterpret_cast<const unsigned char*>(mKey),
                        iv) != 1)
    {
        qDebug() << "EVP_DecryptInit_ex failed!";
        return;
    }

    int outLen;
    //int totalLen = 0;
    if (EVP_DecryptUpdate(mCtx, reinterpret_cast<unsigned char*>(decryptedData.data()), &outLen,
                        reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size()) != 1)
    {
        qDebug() << "EVP_DecryptUpdate failed!";
        return;
    }

    //totalLen += outLen;

    if (EVP_DecryptFinal_ex(mCtx, reinterpret_cast<unsigned char*>(decryptedData.data()) + outLen, &outLen) != 1)
    {
        qDebug() << "EVP_DecryptFinal_ex failed!";
        return;
    }

    // totalLen += outLen;
    // decryptedData.resize(totalLen);
}

bool Decryptor::LoadKey(const QString& filePath)
{
    std::string path = filePath.toStdString();

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Error: cipher - file open: " << path << std::endl;
        return false;
    }

    std::vector<uint8_t> vec((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    if (vec.size() != 32)
    {
        std::cerr << "Error: generate key failed" << std::endl;
        return false;
    }

    auto key = reinterpret_cast<unsigned char*>(vec.data());
    memcpy(mKey, key, 32);

    return true;
}
