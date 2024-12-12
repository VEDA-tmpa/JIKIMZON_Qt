#ifndef JIKIMZON_BODY_H
#define JIKIMZON_BODY_H

#include <cstdint>
#include <cstring>
#include <vector>
#include <QByteArray>
#include "ISerializeInterface.h"

namespace frame
{
    class Body : virtual public common::ISerializeInterface
    {
    public:
        Body() = default;
        Body(std::vector<uint8_t>& image);
        ~Body() = default;

        std::vector<uint8_t> Serialize() const override;
        void Serialize(std::vector<uint8_t>& OUT buffer) const override;
        void Deserialize(std::vector<uint8_t>& buffer) override;
        void Deserialize(QByteArray& buffer) override;

        void SetImage(std::vector<uint8_t>& image);
        std::vector<uint8_t> GetImage() const;

    private:
        std::vector<uint8_t> mImage;
    };
}

#endif // JIKIMZON_BODY_H
