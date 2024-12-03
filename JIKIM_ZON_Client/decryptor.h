#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include <QByteArray>

class Decryptor
{
public:
    // Decryptor();
    // static QByteArray decryptChaCha20(const QByteArray &encryptedData,
    //                                   const QByteArray &key, const QByteArray &nonce);
    // static QByteArray readKeyFromFile(const QString &filename);
    static bool initialize(const QString &filePath);
    static QByteArray decryptChaCha20(const QByteArray &encryptedData);

    static QByteArray getKey();
    static QByteArray getNonce();

private:
    static QByteArray key;
    static QByteArray nonce;
};

#endif // DECRYPTOR_H
