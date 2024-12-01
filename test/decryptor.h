#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include <QByteArray>
#include <vector>
#include <openssl/evp.h>

class Decryptor {
public:
    // Decryptor(const QByteArray& key);
    // Decryptor(const QByteArray& key, const QByteArray& nonce);
    explicit Decryptor(const QByteArray& key);
    QByteArray decrypt(const QByteArray& encryptedData);

private:
    QByteArray key;
    QByteArray nonce;
};

// bool loadKeyAndNonce(const QString& filePath, QByteArray& key, QByteArray& nonce);
bool loadKey(const QString& filePath, QByteArray& key);

#endif // DECRYPTOR_H
