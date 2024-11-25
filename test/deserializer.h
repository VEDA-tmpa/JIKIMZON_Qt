#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <vector>
#include <cstdint>
#include <cstring>
#include "Frame.h"

namespace frame {

//네트워크 바이트 순서(빅 엔디안)
inline uint32_t toNetworkEndian(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x000000FF) << 24);
}

inline uint16_t toNetworkEndian(uint16_t value) {
    return ((value & 0xFF00) >> 8) |
           ((value & 0x00FF) << 8);
}

//네트워크 바이트 순서(빅 엔디안)의 값을 호스트 바이트 순서(리틀 엔디안)
inline uint32_t toHostEndian(uint32_t value) {
    // 네트워크 -> 리틀 엔디안(호스트) 변환도 동일한 방식
    return toNetworkEndian(value);
}

inline uint16_t toHostEndian(uint16_t value) {
    return toNetworkEndian(value);
}

class Deserializer {
public:
    // 역직렬화 메소드
    static bool DeserializeHeader(const std::vector<uint8_t>& buffer, Header& header);

    static bool DeserializeBody(const std::vector<uint8_t>& buffer, Body& body);

    static bool DeserializeFrame(const std::vector<uint8_t>& buffer, Frame& frame);
};

} // namespace frame

#endif // DESERIALIZER_H
