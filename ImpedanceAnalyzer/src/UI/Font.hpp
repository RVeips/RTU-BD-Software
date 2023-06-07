#pragma once

// All Fonts
#include <UI/font_18.h>

//
#include <UI/Image.hpp>
#include <Utils/Compression.hpp>

namespace GFX {

    class Font {
    public:
#pragma pack(1)
        struct Char {
            uint16_t width;
            uint16_t height;
            int16_t xAdvance;
            int16_t xOffset;
            int16_t yOffset;
            uint16_t _; // Alignment
            Compression::RLE<uint16_t> data[];
        };
#pragma pack()

    public:
        Font(int baseHeight,
             int lineHeight,
             int lineHeightCorrection,
             const uint16_t *offsetTable,
             const uint16_t *dataTable,
             uint8_t charCount);
        const Char *GetChar(char c);

        inline int LineHeight() const {
            return m_LineHeight + m_LineHeightCorrection;
        }

        inline int RawLineHeight() const {
            return m_LineHeight;
        }

        inline int LineHeightCorrection() const {
            return m_LineHeightCorrection;
        }

        inline int BaseHeight() const {
            return m_BaseHeight;
        }

        inline uint8_t CharCount() const {
            return m_CharCount;
        }

    private:
        int m_BaseHeight;
        int m_LineHeight;
        int m_LineHeightCorrection;
        const uint16_t *m_OffsetTable;
        const uint16_t *m_DataTable;
        uint8_t m_CharCount;
    };

//////////////////////////////////////////
#define CONSTRUCT_FONT(fontname)               \
    Font(font_baseheight_##fontname,           \
         font_lineheight_##fontname,           \
         font_lineheightcorrection_##fontname, \
         font_offset_##fontname,               \
         font_data_##fontname,                 \
         sizeof(font_offset_##fontname) / sizeof(font_data_##fontname[0]))

    namespace Fonts {

        namespace Size18 {
            static Font main = CONSTRUCT_FONT(18);
        }

    } // namespace Fonts
} // namespace GFX
