#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace frame {

enum class ImageFormat { RAW, JPEG, PNG };

struct HeaderStruct {
    uint32_t frameId;
    uint32_t bodySize;
    uint32_t imageWidth;
    uint32_t imageHeight;
    ImageFormat imageFormat;
    char timestamp[20]; // 예시 시간
    uint8_t padding1[3];
    uint8_t padding2;
};

class Header {
public:
    void SetHeader(const HeaderStruct& headerStruct)
    {
        mHeaderStruct = headerStruct;
    }

    void Serialize(std::vector<uint8_t>& outBuffer) const
    {
        outBuffer.resize(sizeof(mHeaderStruct));
        std::memcpy(outBuffer.data(), &mHeaderStruct, sizeof(mHeaderStruct));
    }

    void Deserialize(const std::vector<uint8_t>& inBuffer)
    {
        std::memcpy(&mHeaderStruct, inBuffer.data(), sizeof(mHeaderStruct));
    }

    const HeaderStruct& GetHeaderStruct() const { return mHeaderStruct; }

private:
    HeaderStruct mHeaderStruct;
};

class Body {
public:
    void SetBody(const std::vector<uint8_t>& bodyData)
    {
        mBodyData = bodyData;
    }

    void Serialize(std::vector<uint8_t>& outBuffer) const
    {
        outBuffer = mBodyData;
    }

    void Deserialize(const std::vector<uint8_t>& inBuffer)
    {
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

    void Serialize(std::vector<uint8_t>& outBuffer) const
    {
        mHeader.Serialize(outBuffer);
        std::vector<uint8_t> bodyBuffer;
        mBody.Serialize(bodyBuffer);
        outBuffer.insert(outBuffer.end(), bodyBuffer.begin(), bodyBuffer.end());
    }

    void Deserialize(const std::vector<uint8_t>& inBuffer)
    {
        size_t headerSize = sizeof(HeaderStruct);
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
