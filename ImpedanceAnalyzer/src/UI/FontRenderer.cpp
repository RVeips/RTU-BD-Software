#include <UI/FontRenderer.hpp>
#include <UI/GFX.hpp>
#include <UI/Renderer.hpp>
#include <Utils/Compression.hpp>
#include <Utils/StringUtils.hpp>
#include <Hardware/Display.hpp>

namespace GFX {

    FontRenderer::FontRenderer(GFX::Font *font) :
        m_Font(font), m_Alignment((uint32_t)GFX::TextAlignment::LEFT | (uint32_t)GFX::TextAlignment::TOP) {
    }

    FontRenderer::FontRenderer(GFX::Font *font, uint32_t alignment) : m_Font(font), m_Alignment(alignment) {
    }

    void FontRenderer::RenderCharToImage(GFX::DynamicImage *dimg, const GFX::Font::Char *rChar, uint32_t textColor, uint32_t bgColor) {
        Compression::RLE<uint16_t> *rle = const_cast<Compression::RLE<uint16_t> *>(rChar->data);

        dimg->SetWidth(rChar->width);
        dimg->SetHeight(rChar->height);

        int decodeIterator = rle->n;
        uint8_t rle_output = rle->value << 2;
        uint8_t last_rle   = rle_output;
        uint32_t color     = 0;
        for (register int i = 0; i < rChar->width * rChar->height; i++) {
            if (!decodeIterator--) {
                rle++;
                decodeIterator = rle->n - 1;
                last_rle       = rle_output;
                rle_output     = rle->value << 2;
            }

            if (last_rle != rle_output) {
                last_rle = rle_output;
                color    = Display::ToPixelColor(GFX::ce_Blend565(bgColor, textColor, rle_output));
            }

            dimg->Data()[i] = color;
        }

        // if (Debug::Active) {
        //     if (Debug::FontRenderer::CharOutlines) {
        //         for (int x = 0; x < dimg->Width(); x++) {
        //             // top
        //             if (dimg->Data()[x] == bgColor)
        //                 dimg->Data()[x] = Display::ToPixelColor(Debug::FontRenderer::CharOutlineColor);

        //             // bottom
        //             if (dimg->Data()[x + dimg->Height() * dimg->Width() - dimg->Width()] == bgColor)
        //                 dimg->Data()[x + dimg->Height() * dimg->Width() - dimg->Width()] =
        //                     Display::ToPixelColor(Debug::FontRenderer::CharOutlineColor);
        //         }
        //         for (int y = 0; y < dimg->Height(); y++) {
        //             // left
        //             if (dimg->Data()[y * dimg->Width()] == bgColor)
        //                 dimg->Data()[y * dimg->Width()] = Display::ToPixelColor(Debug::FontRenderer::CharOutlineColor);

        //             // right
        //             if (dimg->Data()[y * dimg->Width() + dimg->Width() - 1] == bgColor)
        //                 dimg->Data()[y * dimg->Width() + dimg->Width() - 1] = Display::ToPixelColor(Debug::FontRenderer::CharOutlineColor);
        //         }
        //     }
        // }
    }

    void FontRenderer::RenderChar(const GFX::Font::Char *rChar, uint32_t textColor, uint32_t bgColor) {
        Compression::RLE<uint16_t> *rle = const_cast<Compression::RLE<uint16_t> *>(rChar->data);

        int decodeIterator = rle->n;
        uint8_t rle_output = rle->value * 4;
        uint8_t last_rle   = rle_output + 1;
        uint32_t color     = 0;
        for (register int i = 0; i < rChar->width * rChar->height; i++) {
            if (!decodeIterator--) {
                rle++;
                decodeIterator = rle->n - 1;
                last_rle       = rle_output;
                rle_output     = rle->value * 4;
            }
            if (decodeIterator < 0) {
                decodeIterator = 0;
            }

            if (last_rle != rle_output) {
                last_rle = rle_output;
                color    = Display::ToPixelColor(GFX::ce_Blend565(bgColor, textColor, rle_output));
            }

            Display::SendPixel(color);
        }
    }

    __used uint16_t renderBuffer[1024];
    __used GFX::DynamicImage charimg;
    void FontRenderer::DrawString(const Vector2<int16_t> &pos, const char *text, uint32_t textColor, uint32_t bgColor) {
        charimg.SetData(renderBuffer);

        int sx = 0;
        int sy = pos.Y() - m_Font->LineHeight();

        if (m_Alignment & GFX::TextAlignment::CENTER_V) {
            sy -= StringHeight(text) / 2;
        } else if (m_Alignment & GFX::TextAlignment::BOTTOM) {
            sy -= StringHeight(text);
        } // TOP case is default

        int textWidth  = StringWidth(text);
        int textHeight = StringHeight(text);

        StringUtils::cb_SplitIntoRegions(text, '\n', strlen(text), [&](int idx, int len) {
            sx = pos.X();
            sy += m_Font->LineHeight();

            const char *textSegment = text + idx;
            int lineWidth           = StringWidth(textSegment, len);

            if (m_Alignment & GFX::TextAlignment::CENTER_H) {
                sx -= lineWidth / 2;
            } else if (m_Alignment & GFX::TextAlignment::RIGHT) {
                sx -= lineWidth;
            } // LEFT case is default

            for (int i = 0; i < len; i++) {
                const GFX::Font::Char *ch = m_Font->GetChar(textSegment[i]);

                int specificOffset = 0;
                if (textSegment[i] == 'j')
                    specificOffset = 1;
                else if (textSegment[i] == 't')
                    specificOffset = -1;

                RenderCharToImage(&charimg, ch, textColor, bgColor);
                UI::Renderer::DrawImage(/*(textWidth - lineWidth) / 2*/ +sx + ch->xOffset + specificOffset,
                                        sy + ch->yOffset + m_Font->LineHeightCorrection(),
                                        charimg);

                sx += (charimg.Width() ? charimg.Width() : ch->xAdvance) + ch->xOffset + specificOffset;
            }
        });
    }

    void FontRenderer::DirectDrawString(const Vector2<int16_t> &pos,
                                        const char *text,
                                        uint32_t textColor,
                                        uint32_t bgColor,
                                        const Rect2i &boundDim) {
        int sx = 0;
        int sy = pos.Y() - m_Font->LineHeight();

        if (m_Alignment & GFX::TextAlignment::CENTER_V) {
            sy -= StringHeight(text) / 2;
        } else if (m_Alignment & GFX::TextAlignment::BOTTOM) {
            sy -= StringHeight(text);
        } // TOP case is default

        StringUtils::cb_SplitIntoRegions(
            text,
            '\n',
            strlen(text),
            [&](int idx, int len) { /////////////////////////////////////////////////////////////////////////////////////
                sx = pos.X();
                sy += m_Font->LineHeight();

                const char *textSegment = text + idx;
                int lineWidth           = StringWidth(textSegment, len);

                if (m_Alignment & GFX::TextAlignment::CENTER_H) {
                    sx -= lineWidth / 2;
                } else if (m_Alignment & GFX::TextAlignment::RIGHT) {
                    sx -= lineWidth;
                }

                for (int i = 0; i < len; i++) {
                    const GFX::Font::Char *ch = m_Font->GetChar(textSegment[i]);

                    int specificOffset = 0;
                    if (textSegment[i] == 'j')
                        specificOffset = 1;
                    else if (textSegment[i] == 't')
                        specificOffset = -1;

                    // (i ? ch->xOffset : 0) - dont offset first characters (can break text cut because of negative offsets)
                    Rect2i charRect =
                        Rect2i{{sx + (i ? ch->xOffset : 0) + specificOffset, sy + ch->yOffset + m_Font->LineHeightCorrection()},
                               {ch->width, ch->height}};

                    // check if char is in bound
                    if (boundDim.Size().X() >= 0 && boundDim.Size().Y() >= 0) {
                        if (!boundDim.Contains(charRect)) {
                            return;
                        }
                    }

                    Display::SetRegion(
                        charRect.Position().X(), charRect.Position().Y(), charRect.BottomRight().X() - 1, charRect.BottomRight().Y() - 1);
                    RenderChar(ch, textColor, bgColor);
                    sx += (i ? 0 : -ch->xOffset) + (ch->width ? ch->width : ch->xAdvance) + ch->xOffset + specificOffset;
                }
            });
    }

    //////

    int FontRenderer::StringWidth(const char *text, int len) {
        int calcLen = (len != -1) ? len : strlen(text);

        int widestIteration = 0;

        StringUtils::cb_SplitIntoRegions(text, '\n', calcLen, [&](int idx, int length) {
            int width               = 0;
            const char *textSegment = text + idx;
            for (int i = 0; i < length; i++) {
                if (textSegment[i] == '\n')
                    continue;

                const GFX::Font::Char *ch = m_Font->GetChar(textSegment[i]);

                if (!ch)
                    ch = m_Font->GetChar('?');

                width += (ch->width ? ch->width : ch->xAdvance) + ch->xOffset;
            }

            if (width > widestIteration) {
                widestIteration = width;
            }
        });

        return widestIteration;
    }

    int FontRenderer::StringHeight(const char *text, int len) {
        int calcLen = (len != -1) ? len : strlen(text);

        int height = m_Font->LineHeight();
        for (int i = 0; i < calcLen; i++) {
            if (text[i] == '\n')
                height += m_Font->LineHeight();
        }

        return height - m_Font->LineHeightCorrection();
    }

} // namespace GFX

GFX::FontRenderer main_font(GFX::FontRenderers::main);