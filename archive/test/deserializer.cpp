#include "deserializer.h"
#include <cstring>
#include <arpa/inet.h>

frame::Deserializer::Deserializer(const QByteArray& key) : decryptor(key) {
    // Decryptor 객체를 key로 초기화
}

bool frame::Deserializer::DeserializeHeader(const std::vector<uint8_t> &buffer, Header &header)
{
    if (buffer.size() < sizeof(HeaderStruct)) {
        std::cerr << "Error: Buffer size is too small for HeaderStruct." << std::endl;
        return false;
    }
    HeaderStruct headerStruct;
    std::memcpy(&headerStruct, buffer.data(), sizeof(HeaderStruct));

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

bool frame::Deserializer::DeserializeFrame(const std::vector<uint8_t>& buffer, Frame& frame) {
    if (buffer.empty()) {
        std::cerr << "Error: Buffer is empty for deserialization." << std::endl;
        return false;
    }

    // 1. QByteArray로 변환 (복호화를 위해)
    QByteArray encryptedData(reinterpret_cast<const char*>(buffer.data()), buffer.size());

    // 2. 복호화 수행
    QByteArray decryptedData = decryptor.decrypt(encryptedData);
    if (decryptedData.isEmpty()) {
        std::cerr << "Error: Decryption failed." << std::endl;
        return false;
    }

    // 3. 복호화된 데이터를 다시 std::vector<uint8_t>로 변환
    std::vector<uint8_t> decryptedBuffer(decryptedData.begin(), decryptedData.end());

    // 4. 복호화된 데이터에서 헤더와 바디를 분리 및 역직렬화
    size_t headerSize = sizeof(HeaderStruct);
    if (decryptedBuffer.size() < headerSize) {
        std::cerr << "Error: Decrypted buffer is too small for deserialization." << std::endl;
        return false;
    }

    // 헤더 처리
    Header header;
    if (!DeserializeHeader(std::vector<uint8_t>(decryptedBuffer.begin(), decryptedBuffer.begin() + headerSize), header)) {
        std::cerr << "Error: Failed to deserialize Header." << std::endl;
        return false;
    }
    std::cout << "Header deserialized successfully!" << std::endl;

    // 바디 처리
    std::vector<uint8_t> bodyBuffer(decryptedBuffer.begin() + headerSize, decryptedBuffer.end());
    Body body;
    if (!DeserializeBody(bodyBuffer, body)) {
        std::cerr << "Error: Failed to deserialize Body." << std::endl;
        return false;
    }
    std::cout << "Body deserialized successfully!" << std::endl;

    // 최종 Frame 설정
    frame = Frame(header, body);
    return true;
}
