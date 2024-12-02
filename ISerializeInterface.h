#ifndef JIKIMZON_ISERIALIZEINTERFACE_H
#define JIKIMZON_ISERIALIZEINTERFACE_H

#define OUT

#include <vector>
#include <cstdint>

namespace common
{
    class ISerializeInterface
    {
    public:
        virtual void Serialize(std::vector<uint8_t>& OUT buffer) const = 0;
        virtual void Deserialize(std::vector<uint8_t>& buffer) = 0;
        virtual std::vector<uint8_t> Serialize() const = 0;
    };
}

#endif // JIKIMZON_ISERIALIZEINTERFACE_H