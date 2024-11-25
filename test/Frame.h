#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace frame {

// enum class ImageFormat : uint8_t { RAW, JPEG, PNG };
enum class ImageFormat : uint8_t {
    RAW = 0,
    JPEG = 1,
    PNG = 2
};


#pragma pack(push, 1)
struct HeaderStruct {
    uint32_t frameId;
    uint32_t bodySize;

    uint16_t imageWidth;
    uint16_t imageHeight;
    ImageFormat imageFormat;
    uint8_t padding1[3];

    char timestamp[19]; // "YYYY-MM-DD HH:MM:SS"
    uint8_t padding2[1];
};
#pragma pack(pop)

class Header {
public:
    void SetHeader(const HeaderStruct& headerStruct) {
        mHeaderStruct = headerStruct;
    }

    void Serialize(std::vector<uint8_t>& outBuffer) const {
        outBuffer.resize(sizeof(mHeaderStruct));
        std::memcpy(outBuffer.data(), &mHeaderStruct, sizeof(mHeaderStruct));
    }

    void Deserialize(const std::vector<uint8_t>& inBuffer) {
        if (inBuffer.size() != sizeof(mHeaderStruct)) {
            throw std::runtime_error("Invalid buffer size for HeaderStruct deserialization");
        }
        std::memcpy(&mHeaderStruct, inBuffer.data(), sizeof(mHeaderStruct));
    }

    const HeaderStruct& GetHeaderStruct() const { return mHeaderStruct; }

private:
    HeaderStruct mHeaderStruct;
};

class Body {
public:
    void SetBody(const std::vector<uint8_t>& bodyData) {
        mBodyData = bodyData;
    }

    void Serialize(std::vector<uint8_t>& outBuffer) const {
        outBuffer = mBodyData;
    }

    void Deserialize(const std::vector<uint8_t>& inBuffer) {
        mBodyData = inBuffer;
    }

    const std::vector<uint8_t>& GetBodyData() const { return mBodyData; }

private:
    std::vector<uint8_t> mBodyData;
};

class Frame {
public:
    Frame() = default;
    Frame(const Header& header, const Body& body) : mHeader(header), mBody(body) {}

    void Serialize(std::vector<uint8_t>& outBuffer) const {
        mHeader.Serialize(outBuffer);
        std::vector<uint8_t> bodyBuffer;
        mBody.Serialize(bodyBuffer);
        outBuffer.insert(outBuffer.end(), bodyBuffer.begin(), bodyBuffer.end());
    }

    void Deserialize(const std::vector<uint8_t>& inBuffer) {
        size_t headerSize = sizeof(HeaderStruct);
        if (inBuffer.size() < headerSize) {
            throw std::runtime_error("Invalid buffer size for Frame deserialization");
        }
        std::vector<uint8_t> headerBuffer(inBuffer.begin(), inBuffer.begin() + headerSize);
        mHeader.Deserialize(headerBuffer);
        std::vector<uint8_t> bodyBuffer(inBuffer.begin() + headerSize, inBuffer.end());
        mBody.Deserialize(bodyBuffer);
    }

    const Header& GetHeader() const { return mHeader; }
    const Body& GetBody() const { return mBody; }

private:
    Header mHeader;
    Body mBody;
};

} // namespace frame

#endif // FRAME_H
