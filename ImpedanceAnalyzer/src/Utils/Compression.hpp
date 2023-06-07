#pragma once

namespace Compression {

#pragma pack(1)
    template<typename T>
    struct RLE {
        T value;
        T n;

        void operator<<(RLE *r) {
            this->value = r->value;
            this->n     = r->n;
        }
    };

#pragma pack(1)
    struct CompressedImage256 {
        uint16_t m_ColorTable[256];
        uint8_t data[];
    };

#pragma pack()

} // namespace Compression
