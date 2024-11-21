#include "Decryptor.h"
#include <QDebug>
#include <openssl/evp.h>

Decryptor::Decryptor(const QByteArray& key) : key(key) {}

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
    if (EVP_DecryptInit_ex(ctx, EVP_chacha20(), nullptr, (unsigned char*)key.data(), nonce.data()) != 1) {
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
