#ifndef JIKIMZON_FRAME_H
#define JIKIMZON_FRAME_H

#include <cstdint>
#include <vector>

#include "ISerializeInterface.h"
#include "header.h"
#include "body.h"

#define OUT

namespace frame
{
    enum ImageFormat : uint8_t
    {
        RAW = 0,
        H264 = 1
    };

    class Frame : virtual public common::ISerializeInterface
	{
	public:
        Frame() = default;
        Frame(Header& header, Body& body);
        ~Frame() = default;

        std::vector<uint8_t> Serialize() const override;
        void Serialize(std::vector<uint8_t>& OUT buffer) const override;
		void Deserialize(std::vector<uint8_t>& buffer) override;

        void SetFrame(Header& header, Body& body);
		const Header& GetHeader() const; 
        const Body& GetBody() const;

	private:
		Header mHeader;
		Body mBody;
	};
}

#endif // JIKIMZON_FRAME_H