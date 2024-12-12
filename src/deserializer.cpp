#include "deserializer.h"
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <arpa/inet.h>
#endif

// Deserializer::Deserializer() {}

bool frame::Deserializer::DeserializeHeader(const std::vector<uint8_t> &buffer, Header &header)
{
    if (buffer.size() < sizeof(HeaderStruct)) {
        std::cerr << "Error: Buffer size is too small for HeaderStruct." << std::endl;
        return false;
    }
    HeaderStruct headerStruct;
    std::memcpy(&headerStruct, buffer.data(), sizeof(HeaderStruct));

    // 엔디안 변환 적용
    // headerStruct.frameId = toHostEndian(headerStruct.frameId);
    // headerStruct.bodySize = toHostEndian(headerStruct.bodySize);
    // headerStruct.imageWidth = toHostEndian(headerStruct.imageWidth);
    // headerStruct.imageHeight = toHostEndian(headerStruct.imageHeight);

    // arpa/inet.h 엔디안 변환 적용
    headerStruct.frameId = ntohl(headerStruct.frameId);
    headerStruct.bodySize = ntohl(headerStruct.bodySize);
    headerStruct.imageWidth = ntohs(headerStruct.imageWidth);
    headerStruct.imageHeight = ntohs(headerStruct.imageHeight);

    header.SetHeader(headerStruct);
    return true;
}

bool frame::Deserializer::DeserializeBody(const std::vector<uint8_t> &buffer, Body &body)
{
    body.SetBody(buffer);  // Body는 단순히 데이터 전체를 담음
    return true;
}

bool frame::Deserializer::DeserializeFrame(const std::vector<uint8_t>& buffer, Frame& frame)
{
    size_t headerSize = sizeof(HeaderStruct);
    if (buffer.size() < headerSize) {
        std::cerr << "Error: Buffer is too small for deserialization." << std::endl;
        return false;
    }

    // Header 역직렬화
    Header header;
    if (!DeserializeHeader(std::vector<uint8_t>(buffer.begin(), buffer.begin() + headerSize), header)) {
        std::cerr << "Error: Failed to deserialize Header." << std::endl;
        return false;
    }
    std::cout << "Header deserialized successfully!" << std::endl;

    // Body 역직렬화
    std::vector<uint8_t> bodyBuffer(buffer.begin() + headerSize, buffer.end());
    Body body;
    if (!DeserializeBody(bodyBuffer, body)) {
        std::cerr << "Error: Failed to deserialize Body." << std::endl;
        return false;
    }
    std::cout << "Body deserialized successfully!" << std::endl;

    frame = Frame(header, body);
    return true;
}
