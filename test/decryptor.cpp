#include "Decryptor.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <openssl/evp.h>

Decryptor::Decryptor(const QByteArray& key)
    : key(key), nonce(QByteArray(12, 0x00)) // 기본 고정된 nonce 값
{
    if (key.size() != 32) { // ChaCha20 키는 256비트(32바이트)여야 함
        qDebug() << "Invalid key size! ChaCha20 requires a 256-bit (32-byte) key.";
    }
}

QByteArray Decryptor::decrypt(const QByteArray& encryptedData) {
    if (encryptedData.isEmpty()) {
        qDebug() << "Encrypted data is empty!";
        return {};
    }

    if (key.size() != 32) {
        qDebug() << "Decryption failed: Invalid key size!";
        return {};
    }

    std::vector<unsigned char> encrypted(encryptedData.begin(), encryptedData.end());
    std::vector<unsigned char> decrypted(encrypted.size());

    qDebug() << "복호화 시작! Data size:" << encrypted.size();

    // OpenSSL 복호화 초기화
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qDebug() << "EVP_CIPHER_CTX_new failed!";
        return {};
    }

    // ChaCha20 복호화 초기화
    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(nonce.data())) != 1) {
        qDebug() << "EVP_DecryptInit_ex failed!";
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 복호화 처리
    int outLen = 0;
    int totalOutLen = 0;
    if (EVP_DecryptUpdate(ctx, decrypted.data(), &outLen, encrypted.data(), encrypted.size()) != 1) {
        qDebug() << "EVP_DecryptUpdate failed!";
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    totalOutLen += outLen;

    // 복호화 종료
    if (EVP_DecryptFinal_ex(ctx, decrypted.data() + totalOutLen, &outLen) != 1) {
        qDebug() << "EVP_DecryptFinal_ex failed! Possible data corruption.";
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    totalOutLen += outLen;

    EVP_CIPHER_CTX_free(ctx);

    qDebug() << "Decryption successful. Total decrypted size:" << totalOutLen;

    // 복호화된 데이터를 QByteArray로 반환
    return QByteArray(reinterpret_cast<char*>(decrypted.data()), totalOutLen);
}

bool loadKey(const QString& filePath, QByteArray& key) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open key file:" << filePath;
        return false;
    }

    key = file.readAll();
    file.close();

    if (key.isEmpty()) {
        qDebug() << "Key file is empty.";
        return false;
    }

    if (key.size() != 32) {
        qDebug() << "Invalid key size. ChaCha20 requires a 256-bit (32-byte) key.";
        return false;
    }

    qDebug() << "Key successfully loaded. Size:" << key.size();
    return true;
}
