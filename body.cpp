#include "body.h"

namespace frame
{
    Body::Body(std::vector<uint8_t>& image)
        : mImage(image)
    {
    }

    void Body::Serialize(std::vector<uint8_t>& OUT buffer) const
    {
        buffer.clear();
        buffer.resize(mImage.size());
        std::memcpy(buffer.data(), mImage.data(), mImage.size());
    }

    std::vector<uint8_t> Body::Serialize() const
    {
        std::vector<uint8_t> buffer(mImage.size());
        this->Serialize(buffer);

        return buffer;
    }

    void Body::Deserialize(std::vector<uint8_t>& buffer)
    {
        mImage.clear();
        mImage.resize(buffer.size());
        std::memcpy(mImage.data(), buffer.data(), buffer.size());
    }

    std::vector<uint8_t> Body::GetImage() const
    {
        return mImage;
    }

    void Body::SetImage(std::vector<uint8_t>& image)
    {
        mImage = image;
    }
}