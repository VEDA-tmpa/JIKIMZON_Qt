#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <arpa/inet.h>
#endif

#include <vector>
#include <cstdint>
#include <cstring>
#include "Decryptor.h"
#include "Frame.h"

namespace frame {

class Deserializer {
public:
    // 생성자에서 복호화기 초기화
    explicit Deserializer(const QByteArray& decryptionKey);

    bool DeserializeFrame(const std::vector<uint8_t>& buffer, Frame& frame);
    bool DeserializeHeader(const std::vector<uint8_t>& buffer, Header& header);
    bool DeserializeBody(const std::vector<uint8_t>& buffer, Body& body);

private:
    Decryptor decryptor;  // Decryptor 객체
};

} // namespace frame

#endif // DESERIALIZER_H
