#include <UI/Font.hpp>
#include <CFXS/Base/Debug.hpp>

namespace GFX {

    Font::Font(int baseHeight,
               int lineHeight,
               int lineHeightCorrection,
               const uint16_t *offsetTable,
               const uint16_t *dataTable,
               uint8_t charCount) :
        m_BaseHeight(baseHeight),
        m_LineHeight(lineHeight),
        m_LineHeightCorrection(lineHeightCorrection),
        m_OffsetTable(offsetTable),
        m_DataTable(dataTable),
        m_CharCount(charCount) {
    }

    const Font::Char *Font::GetChar(char c) {
        if (!(c >= 32 && (c - 32) < m_CharCount)) {
            CFXS_printf("GetChar char not found %d %d(-32) %d(cc)\n", c, c - 32, m_CharCount);
            return reinterpret_cast<const Char *>(&m_DataTable[0]);
        }

        uint16_t offset = m_OffsetTable[c - 32];

        const Char *ch = reinterpret_cast<const Char *>(&m_DataTable[offset]);

        return ch;
    }

} // namespace GFX
