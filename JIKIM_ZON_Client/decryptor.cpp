#include "decryptor.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <QFile>
#include <QDebug>

QByteArray Decryptor::key;
QByteArray Decryptor::nonce;

bool Decryptor::initialize(const QString &filePath)
{
    QString keyFilePath = ":/keyfile/keyfile.bin";
    QFile file(keyFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open key file:" << keyFilePath;
        return false;
    }
    key = file.readAll();
    file.close();

    nonce = QByteArray(12, 0);  // 12바이트 nonce, 0으로 초기화
    return true;
}

QByteArray Decryptor::getKey()
{
    return key;
}

QByteArray Decryptor::getNonce()
{
    return nonce;
}

QByteArray Decryptor::decryptChaCha20(const QByteArray &encryptedData)
{

    QByteArray decryptedData;
    decryptedData.resize(encryptedData.size());

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qDebug() << "Failed to create EVP_CIPHER_CTX";
        return QByteArray();
    }


     // ChaCha20 복호화 초기화
    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr, reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(nonce.data())) != 1) {
        qDebug() << "Failed to initialize decryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    int outLen;
     // 복호화 업데이트
    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(decryptedData.data()), &outLen,
                          reinterpret_cast<const unsigned char*>(encryptedData.data()), encryptedData.size()) != 1) {
        qDebug() << "Failed to decrypt data";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    int finalLen;
    // 복호화 최종화
    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(decryptedData.data()) + outLen, &finalLen) != 1) {
        qDebug() << "Failed to finalize decryption";
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }

    decryptedData.resize(outLen + finalLen); // 최종 데이터 크기 조정
    EVP_CIPHER_CTX_free(ctx); // 메모리 해제

    return decryptedData; // 복호화된 데이터 반환
}

// QByteArray Decryptor::readKeyFromFile(const QString &filename)
// {
//     QFile file(filename);
//     if (!file.open(QIODevice::ReadOnly)) {
//         qDebug() << "Failed to open key file:" << filename;
//         return QByteArray();
//     }
//     QByteArray key = file.readAll();
//     file.close();
//     return key;
// }
