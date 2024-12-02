#include "frame.h"

#include <cstdint>
#include <iostream>
#include <cstring>

namespace frame
{
    Frame::Frame(Header& header, Body& body)
        : mHeader(header), mBody(body)
    {
    }

	void Frame::Serialize(std::vector<uint8_t>& OUT buffer) const
    {
        if (mHeader.GetBodySize() != mBody.GetImage().size())
        {
            throw std::runtime_error("Header body size is not equal to Body image size");
        }

        std::vector<uint8_t> headerBuffer;
        std::vector<uint8_t> bodyBuffer;

        mHeader.Serialize(headerBuffer); // == headerBuffer = mHeader.Serialize();
        mBody.Serialize(bodyBuffer);     // == bodyBuffer = mBody.Serialize();

        buffer.clear();

        buffer.insert(buffer.end(), headerBuffer.begin(), headerBuffer.end());
        buffer.insert(buffer.end(), bodyBuffer.begin(), bodyBuffer.end());
    }

    std::vector<uint8_t> Frame::Serialize() const
    {
        std::vector<uint8_t> buffer;
        this->Serialize(buffer);
        return buffer;
    }

    void Frame::Deserialize(std::vector<uint8_t>& buffer)
    {
        if (buffer.size() < sizeof(HeaderStruct))
        {
            throw std::runtime_error("Buffer size is too small for a valid Frame");
        }

        std::vector<uint8_t> headerBuffer(buffer.begin(), buffer.begin() + sizeof(HeaderStruct));
        mHeader.Deserialize(headerBuffer); 


        //size_t bodySize = mHeader.GetBodySize();

        if (buffer.size() != sizeof(HeaderStruct) + mHeader.GetBodySize())
        {
            throw std::runtime_error("Buffer size is not equal to the sum of Header and Body size");
        }


        std::vector<uint8_t> bodyBuffer(buffer.begin() + sizeof(HeaderStruct), buffer.end());
        mBody.Deserialize(bodyBuffer);
    }

	const Header& Frame::GetHeader() const
	{
		return mHeader;
	}
	const Body& Frame::GetBody() const
	{
		return mBody;
	}

    void Frame::SetFrame(Header& header, Body& body)
    {
        mHeader = header;
        mBody = body;
    }
}