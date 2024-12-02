#include "decryptor.h"

#include <QFile>
#include <QDir>
#include <QDebug>

Decryptor::Decryptor()
{
    mCtx = EVP_CIPHER_CTX_new();
    if (!mCtx) {
        qDebug() << "EVP_CIPHER_CTX_new failed!";
    }

    QString filepath = QDir::currentPath() + "/res/keyfile.bin";
    LoadKey(filepath);
}

Decryptor::~Decryptor()
{
    if (mCtx)
    {
        EVP_CIPHER_CTX_free(mCtx);
    }
}

void Decryptor::Decrypt(QString& nounceStr, const QByteArray& encryptedData, OUT QByteArray& decryptedData)
{   
    QString nounce = nounceStr;
    if (nounceStr.length() > 12)
    {
        nounce = nounceStr.right(12);
    }

    decryptedData.clear();

    if (EVP_DecryptInit_ex(mCtx, EVP_chacha20(), nullptr,
                           reinterpret_cast<const unsigned char*>(mKey.data()),
                           reinterpret_cast<const unsigned char*>(nounce.toStdString().c_str())) != 1)
    {
        qDebug() << "EVP_DecryptInit_ex failed!";
        return;
    }

    int outLen;
    if (EVP_DecryptUpdate(mCtx, reinterpret_cast<unsigned char*>(decryptedData.data()), &outLen,
                          reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size()) != 1)
    {
        qDebug() << "EVP_DecryptUpdate failed!";
        return;
    }

    if (EVP_DecryptFinal_ex(mCtx, reinterpret_cast<unsigned char*>(decryptedData.data()) + outLen, &outLen) != 1)
    {
        qDebug() << "EVP_DecryptFinal_ex failed!";
        return;
    }
}

bool Decryptor::LoadKey(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open key file.";
        return false;
    }

    mKey = file.readAll();
    file.close();

    if (mKey.isEmpty()) {
        qDebug() << "Key file is empty.";
        return false;
    }

    return true;
}