#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <opencv2/opencv.hpp>  // OpenCV Mat 포함

#include <arpa/inet.h>

namespace frame {

enum class ImageFormat : uint8_t { RAW, JPEG, PNG };

#pragma pack(push, 1)
struct HeaderStruct {
    uint32_t frameId;
    uint32_t bodySize;
    uint16_t imageWidth;
    uint16_t imageHeight;
    ImageFormat imageFormat;
    uint8_t padding1[3];
    char timestamp[19];
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

    bool Deserialize(const std::vector<uint8_t>& inBuffer) {
        if (inBuffer.size() != sizeof(mHeaderStruct)) {
            throw std::runtime_error("Invalid buffer size for HeaderStruct deserialization");
        }

        std::memcpy(&mHeaderStruct, inBuffer.data(), sizeof(mHeaderStruct));

        // arpa/inet.h 엔디안 변환 적용
        mHeaderStruct.frameId = ntohl(mHeaderStruct.frameId);
        mHeaderStruct.bodySize = ntohl(mHeaderStruct.bodySize);
        mHeaderStruct.imageWidth = ntohs(mHeaderStruct.imageWidth);
        mHeaderStruct.imageHeight = ntohs(mHeaderStruct.imageHeight);
    }

    // frame::Header 클래스에 타임스탬프 반환하는 메서드가 있어야 합니다.
    std::string GetTimestamp() const {
        // 타임스탬프를 반환하는 로직을 구현
        return timestamp;
    }

    const HeaderStruct& GetHeaderStruct() const { return mHeaderStruct; }

private:
    HeaderStruct mHeaderStruct;
    std::string timestamp; // timestamp 변수 추가
};

class Body {
public:
    void SetBody(const std::vector<uint8_t>& bodyData) {
        mBodyData = bodyData;
    }

    void Serialize(std::vector<uint8_t>& outBuffer) const {
        outBuffer = mBodyData;
    }

    bool Deserialize(const std::vector<uint8_t>& inBuffer) {
        std::vector<uint8_t> buffer(inBuffer.begin(), inBuffer.end());
        mBodyData = inBuffer;
    }

    const std::vector<uint8_t>& GetBodyData() const { return mBodyData; }

private:
    std::vector<uint8_t> mBodyData;
};

class Frame {
public:
    Frame() = default;

    // Header와 Body만 받는 생성자 추가
    Frame(const Header& header, const Body& body)
        : mHeader(header), mBody(body) {}

    Frame(const Header& header, const Body& body, const cv::Mat& image)
        : mHeader(header), mBody(body), data(image) {}

    void Serialize(std::vector<uint8_t>& outBuffer) const {
        mHeader.Serialize(outBuffer);
        std::vector<uint8_t> bodyBuffer;
        mBody.Serialize(bodyBuffer);
        outBuffer.insert(outBuffer.end(), bodyBuffer.begin(), bodyBuffer.end());

        // 이미지 데이터를 직렬화하여 추가
        if (!data.empty()) {
            std::vector<uint8_t> imageData;
            // OpenCV Mat을 바이트 배열로 변환
            cv::imencode(".jpg", data, imageData);  // JPEG 형식으로 인코딩
            outBuffer.insert(outBuffer.end(), imageData.begin(), imageData.end());
        }
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

        // 이미지 데이터 추출 (여기서는 JPEG로 가정)
        std::vector<uint8_t> imageData(inBuffer.begin() + headerSize + bodyBuffer.size(), inBuffer.end());
        data = cv::imdecode(imageData, cv::IMREAD_COLOR);  // JPEG 디코딩
    }

    const Header& GetHeader() const { return mHeader; }
    const Body& GetBody() const { return mBody; }
    const cv::Mat& GetData() const { return data; }

private:
    Header mHeader;
    Body mBody;
    cv::Mat data;  // 영상 데이터를 저장하는 Mat 객체
};

} // namespace frame

#endif // FRAME_H
