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

    QString filepath = ":/res/res/keyfile_tmp.bin";
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
    decryptedData.clear();
    decryptedData.resize(encryptedData.size());

    unsigned char iv[12];
    std::memcpy(iv, reinterpret_cast<const unsigned char*>(nounceStr.right(12).toStdString().c_str()), 12);

    if (EVP_DecryptInit_ex(mCtx, EVP_chacha20(), nullptr,
                        reinterpret_cast<const unsigned char*>(mKey.data()),
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

bool Decryptor::LoadKey(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open key file.";
        return false;
    }

    mKey = file.readAll();
    file.close();

    if (mKey.isEmpty())
    {
        qDebug() << "Key file is empty.";
        return false;
    }

    if (mKey.size() != 32)
    {
        qDebug() << "Invalid key size: " << mKey.size();
        return false;
    }

    return true;
}
