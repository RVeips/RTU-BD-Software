#pragma once

#include <UI/Image.hpp>
#include <UI/Font.hpp>
#include <UI/Vector2.hpp>
#include <UI/Rect.hpp>
#include <UI/GFX.hpp>

#define DEBUG_FR
#ifdef DEBUG_FR
    #define FRLOG(...)        \
        NAFT_printf("[FR] "); \
        NAFT_printf(__VA_ARGS__)
#else
    #define FRLOG(...)
#endif

namespace GFX {
#pragma pack(4)
    class FontRenderer {
    public:
        FontRenderer(GFX::Font *font);
        FontRenderer(GFX::Font *font, uint32_t alignment);

        void RenderCharToImage(GFX::DynamicImage *dimg,
                               const GFX::Font::Char *rChar,
                               uint32_t textColor = 0xFFFFFF,
                               uint32_t bgColor   = 0x000000);

        void RenderChar(const GFX::Font::Char *rChar, uint32_t textColor = 0xFFFFFF, uint32_t bgColor = 0x000000);

        void DrawString(const Vector2<int16_t> &pos, const char *text, uint32_t textColor = 0xFFFFFF, uint32_t bgColor = 0x000000);

        void DirectDrawString(const Vector2<int16_t> &pos,
                              const char *text,
                              uint32_t textColor     = 0xFFFFFF,
                              uint32_t bgColor       = 0x000000,
                              const Rect2i &boundDim = {{-1}, {-1}});

        int StringWidth(const char *text, int len = -1);
        int StringHeight(const char *text, int len = -1);

        Vector2<int> StringSize(char *str) {
            return {StringWidth(str), StringHeight(str)};
        }

        //// Getter & Virtual Riot

        inline void SetAlignment(uint32_t alignment) {
            m_Alignment = alignment;
        }

        inline void SetFont(GFX::Font *font) {
            m_Font = font;
        }

        inline GFX::Font *Font() const {
            return m_Font;
        }

        ~FontRenderer() = default;

    private:
        GFX::Font *m_Font;
        uint32_t m_Alignment;
    };
#pragma pack()

    namespace FontRenderers {
        static FontRenderer main = FontRenderer(&GFX::Fonts::Size18::main, (uint32_t)GFX::TextAlignment::LEFT);
    }
} // namespace GFX

extern GFX::FontRenderer main_font;