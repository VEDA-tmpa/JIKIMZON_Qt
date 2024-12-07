#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <vector>
#include <cstdint>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

#include "frame.h"

namespace frame {

class Deserializer {
public:
    // 역직렬화 메소드
    static bool DeserializeHeader(const std::vector<uint8_t>& buffer, Header& header);

    static bool DeserializeBody(const std::vector<uint8_t>& buffer, Body& body);

    static bool DeserializeFrame(const std::vector<uint8_t> &buffer, Frame &frame);
};

} // namespace frame

#endif // DESERIALIZER_H
