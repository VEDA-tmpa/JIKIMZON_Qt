#ifndef DECRYPTOR_H
#define DECRYPTOR_H

#include <QByteArray>
#include <vector>

class Decryptor {
public:
    Decryptor(const QByteArray& key);
    QByteArray decrypt(const QByteArray& encryptedData);

private:
    QByteArray key;
};

#endif // DECRYPTOR_H
