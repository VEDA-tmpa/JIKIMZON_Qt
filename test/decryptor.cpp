#include "Decryptor.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <openssl/evp.h>

// Decryptor::Decryptor(const QByteArray& key) : key(key) {}
//생성자
// Decryptor::Decryptor(const QByteArray& key, const QByteArray& nonce)
//     : key(key), nonce(nonce) {}
Decryptor::Decryptor(const QByteArray& key)
    : key(key), nonce(QByteArray(12, 0x00)) // 고정된 nonce 값
{}

//암호화 해제
QByteArray Decryptor::decrypt(const QByteArray& encryptedData) {
    std::vector<unsigned char> encrypted(encryptedData.begin(), encryptedData.end());
    std::vector<unsigned char> decrypted(encrypted.size());

    qDebug() << "복호화 시작!!!";

    // ChaCha20 복호화
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        qDebug() << "EVP_CIPHER_CTX_new failed!";
        return {};
    }

    // nonce 설정 (서버와 동일한 방식으로 12바이트로 초기화)
    std::vector<unsigned char> nonce(12, 0x00);

    // ChaCha20 복호화 초기화
    // if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr, (unsigned char*)key.data(), nonce.data()) != 1) {
    //     qDebug() << "EVP_DecryptInit_ex failed!";
    //     EVP_CIPHER_CTX_free(ctx);
    //     return {};
    // }
    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr,
                           reinterpret_cast<const unsigned char*>(key.data()),
                           reinterpret_cast<const unsigned char*>(nonce.data())) != 1) {
        qDebug() << "EVP_DecryptInit_ex failed!";
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 복호화 처리
    int outLen;
    if (EVP_DecryptUpdate(ctx, decrypted.data(), &outLen, encrypted.data(), encrypted.size()) != 1) {
        qDebug() << "EVP_DecryptUpdate failed!";
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 복호화 종료
    if (EVP_DecryptFinal_ex(ctx, decrypted.data() + outLen, &outLen) != 1) {
        qDebug() << "EVP_DecryptFinal_ex failed!";
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    qDebug() << "Decryption successful. Data size: " << decrypted.size();

    // 복호화된 데이터를 QByteArray로 반환
    return QByteArray(reinterpret_cast<char*>(decrypted.data()), encrypted.size());
}

// // 키를 텍스트파일에서 읽어오는 함수
// bool loadKey(const QString& filePath, QByteArray& key) {
//     QFile file(filePath);
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
//         qDebug() << "Failed to open key file:" << filePath;
//         return false;
//     }

//     QTextStream in(&file);
//     QString line;
//     while (in.readLineInto(&line)) {
//         if (line.startsWith("key:")) {
//             key = QByteArray::fromHex(line.mid(4).trimmed().toUtf8());
//         }
//     }
//     file.close();

//     if (key.isEmpty()) {
//         qDebug() << "Key is missing in file.";
//         return false;
//     }

//     return true;
// }

bool loadKey(const QString& filePath, QByteArray& key) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open key file:" << filePath;
        return false;
    }

    // 파일의 전체 내용을 읽어서 key에 저장
    key = file.readAll();
    file.close();

    if (key.isEmpty()) {
        qDebug() << "Key file is empty.";
        return false;
    }

    qDebug() << "Key successfully loaded. Size:" << key.size();
    return true;
}
