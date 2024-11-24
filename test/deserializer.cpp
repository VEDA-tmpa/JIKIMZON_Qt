#include "deserializer.h"
#include <cstring>

// Deserializer::Deserializer() {}

bool frame::Deserializer::DeserializeHeader(const std::vector<uint8_t> &buffer, Header &header)
{
    if (buffer.size() < sizeof(HeaderStruct)) {
        std::cerr << "Error: Buffer size is too small for HeaderStruct." << std::endl;
        return false;
    }
    HeaderStruct headerStruct;
    std::memcpy(&headerStruct, buffer.data(), sizeof(HeaderStruct));
    header.SetHeader(headerStruct);
    return true;
}

bool frame::Deserializer::DeserializeBody(const std::vector<uint8_t> &buffer, Body &body)
{
    body.SetBody(buffer);  // Body는 단순히 데이터 전체를 담음
    return true;
}

bool frame::Deserializer::DeserializeFrame(const std::vector<uint8_t> &buffer, Frame &frame)
{
    // Header와 Body를 분리해서 역직렬화
    size_t headerSize = sizeof(HeaderStruct);
    if (buffer.size() < headerSize) {
        return false;
    }

    // Header 역직렬화
    Header header;
    if (!DeserializeHeader(std::vector<uint8_t>(buffer.begin(), buffer.begin() + headerSize), header)) {
        return false;
    }

    // Body 역직렬화
    std::vector<uint8_t> bodyBuffer(buffer.begin() + headerSize, buffer.end());
    Body body;
    if (!DeserializeBody(bodyBuffer, body)) {
        return false;
    }

    // auto headerBuffer = std::vector<uint8_t>(buffer.data(), buffer.data() + headerSize);
    // if (!DeserializeHeader(headerBuffer, header)) {
    //     return false;
    // }


    // Frame 객체에 Header와 Body 설정
    frame = Frame(header, body);
    return true;
}
