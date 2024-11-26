#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <cstdint>
#include <cstring>
#include <opencv2/opencv.hpp>  // OpenCV Mat 포함

namespace frame {

enum class ImageFormat : uint8_t { RAW, JPEG, PNG };

#pragma pack(push, 1) //1바이트 정렬이 적용되어 모든 멤버가 패딩 없이 바로 뒤에 배치
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
#pragma pack(pop) //#pragma pack(push, 1)으로 변경한 정렬 설정을 원래 상태로 되돌림

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

        // arpa/inet.h 엔디안 변환 적용
        mHeaderStruct.frameId = ntohl(mHeaderStruct.frameId);
        mHeaderStruct.bodySize = ntohl(mHeaderStruct.bodySize);
        mHeaderStruct.imageWidth = ntohs(mHeaderStruct.imageWidth);
        mHeaderStruct.imageHeight = ntohs(mHeaderStruct.imageHeight);
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

    // void Deserialize(const std::vector<uint8_t>& inBuffer) {
    //     mBodyData = inBuffer;
    // }

    void Deserialize(const std::vector<uint8_t>& inBuffer) {
        // 0x000001 시작 코드 추가
        std::vector<uint8_t> h264_data_with_start_code;
        h264_data_with_start_code.push_back(0x00);
        h264_data_with_start_code.push_back(0x00);
        h264_data_with_start_code.push_back(0x01);  // NAL unit을 위한 start code 추가

        // 입력 버퍼를 시작 코드 뒤에 추가
        h264_data_with_start_code.insert(h264_data_with_start_code.end(), inBuffer.begin(), inBuffer.end());

        // 시작 코드가 추가된 데이터를 mBodyData에 저장
        mBodyData = h264_data_with_start_code;
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

        // Header 데이터 읽기
        std::vector<uint8_t> headerBuffer(inBuffer.begin(), inBuffer.begin() + headerSize);
        mHeader.Deserialize(headerBuffer);
        std::vector<uint8_t> bodyBuffer(inBuffer.begin() + headerSize, inBuffer.end());
        mBody.Deserialize(bodyBuffer);

        // raw 이미지 데이터 추출
        std::vector<uint8_t> rawData(inBuffer.begin() + headerSize + bodyBuffer.size(), inBuffer.end());
        data = cv::Mat(rawData.size() / 3, 1280, CV_8UC3, rawData.data()); // raw 이미지로 처리
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
