#ifndef JIKIMZON_DECRYPTOR_H
#define JIKIMZON_DECRYPTOR_H

#include <QByteArray>
#include <vector>
#include <cstdint>
#include <openssl/evp.h>

#define OUT

class Decryptor {
public:
    Decryptor();
    ~Decryptor();

    void EncryptData(std::string& timestamp, std::vector<uint8_t>& src, int size, std::vector<uint8_t>& OUT dest);
    void Decrypt(QString& nounce, const QByteArray& encryptedData, OUT QByteArray& decryptedData);
    void Decrypt(QString& nounce, std::vector<uint8_t>& encryptedData, OUT std::vector<uint8_t>& decryptedData);
    bool LoadKey(const QString& filePath);

private:
    EVP_CIPHER_CTX* mCtx;
    unsigned char mKey[32];
    QByteArray mNonce;
};

#endif // JIKIMZON_DECRYPTOR_H
