#ifndef JIKIMZON_DECRYPTOR_H
#define JIKIMZON_DECRYPTOR_H

#include <QByteArray>
#include <vector>
#include <openssl/evp.h>

#define OUT

class Decryptor {
public:
    Decryptor();
    ~Decryptor();

    void Decrypt(QString& nounce, const QByteArray& encryptedData, OUT QByteArray& decryptedData);
    bool LoadKey(const QString& filePath);

private:
    EVP_CIPHER_CTX* mCtx;
    QByteArray mKey;
    QByteArray mNonce;
};

#endif // JIKIMZON_DECRYPTOR_H
