#include "frame.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")

#elif defined(__linux__) || defined(__APPLE__) && defined(__MACH__) // linux, mac
    #include <arpa/inet.h>
    
#else
    #error "Unsupported platform"
#endif

#include <QtEndian>
#include <QByteArray>
#include <iostream>
#include <cstring>
#include <cstring>

//네트워크는 big-endian이 사용됨
//hton : to big-endian
//ntoh : to little-endian

namespace frame
{
    Header::Header(HeaderStruct& header)
        : mHeader(header)
    {
    }
    
    void Header::Serialize(std::vector<uint8_t>& OUT buffer) const
    {
        buffer.clear();
        buffer.resize(sizeof(HeaderStruct));

        HeaderStruct header = mHeader;
        header.frameId = htonl(header.frameId);
        header.bodySize = htonl(header.bodySize);
        header.imageWidth = htons(header.imageWidth);
        header.imageHeight = htons(header.imageHeight);

        std::memcpy(buffer.data(), reinterpret_cast<void*>(&header), sizeof(HeaderStruct));
    }

    std::vector<uint8_t> Header::Serialize() const
    {
        std::vector<uint8_t> buffer(sizeof(HeaderStruct));

        this->Serialize(buffer);

        return buffer;
    }

    void Header::Deserialize(std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < sizeof(HeaderStruct))
        {
            throw std::runtime_error("Buffer size is too small for a valid Header.");
        }

        HeaderStruct header;
        std::memcpy(&header, buffer.data(), sizeof(HeaderStruct));

        header.frameId = ntohl(header.frameId);
        header.bodySize = ntohl(header.bodySize);
        header.imageWidth = ntohs(header.imageWidth);
        header.imageHeight = ntohs(header.imageHeight);

        mHeader = header;
    }

    void Header::Deserialize(QByteArray& buffer)
    {
        if (buffer.size() < sizeof(HeaderStruct))
        {
            throw std::runtime_error("Buffer size is too small for a valid Header.");
        }

        HeaderStruct header;
        std::memcpy(&header, buffer.data(), sizeof(HeaderStruct));
        
        header.frameId = ntohl(header.frameId);
        header.bodySize = ntohl(header.bodySize);
        header.imageWidth = ntohs(header.imageWidth);
        header.imageHeight = ntohs(header.imageHeight);

        mHeader = header;
    }

    uint32_t Header::GetFrameId() const
    {
        return mHeader.frameId;
    }

    uint32_t Header::GetBodySize() const
    {
        return mHeader.bodySize;
    }

    uint16_t Header::GetImageWidth() const
    {
        return mHeader.imageWidth;
    }

    uint16_t Header::GetImageHeight() const
    {
        return mHeader.imageHeight;
    }

    uint8_t Header::GetImageFormat() const
    {
        return mHeader.imageFormat;
    }

    std::string Header::GetTimestamp() const
    {
        return std::string(mHeader.timestamp);
    }

    void Header::SetHeader(HeaderStruct& header)
    {
        mHeader = header;
    }
}
