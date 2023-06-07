#pragma once

#include <UI/GFX.hpp>
#include <UI/Rect.hpp>
#include <UI/Vector2.hpp>
#include <UI/FontRenderer.hpp>
#include <Hardware/Display.hpp>
#include <CFXS/Base/Math.hpp>

#define CONSTEXPR // constexpr
#define SWAP_IF_BIGGER(a, b) \
    if (a > b) {             \
        auto tmp = a;        \
        a        = b;        \
        b        = tmp;      \
    }

namespace UI {
    class Renderer {
    public:
        Renderer()  = default;
        ~Renderer() = default;

        static void DrawPixel(int x, int y, uint32_t color) {
            Display::SetRegion(x, y, x, y);
            Display::SendPixel(Display::ToPixelColor(color));
        }

        static void DrawPixel_RawColor(int x, int y, uint16_t color) {
            Display::SetRegion(x, y, x, y);
            Display::SendPixel(color);
        }

        static void Clear(uint32_t clearColor = 0x000000) {
            DrawRect({{0, 0}, {Display::GetWidth(), Display::GetHeight()}}, clearColor);
        }

        template<class BitmapType>
        static void
        DrawBitmap(int x1, int y1, const BitmapType *bmp, uint32_t bgColor, uint32_t color, bool flipXY = false, bool invertX = false) {
            Display::SetRegion(x1, y1, x1 + bmp->Width() - 1, y1 + bmp->Height() - 1);

            auto pColor   = Display::ToPixelColor(color);
            auto pBgColor = Display::ToPixelColor(bgColor);

            if (flipXY) {
                for (register int y = 0; y < bmp->Height(); y++) {
                    for (register int x = 0; x < bmp->Width(); x++) {
                        Display::SendPixel(bmp->Pixel(y + x * bmp->Height()) ? pColor : pBgColor);
                    }
                }
            } else {
                if (invertX) {
                    for (register int y = 0; y < bmp->Height(); y++) {
                        for (register int x = bmp->Width() - 1; x >= 0; x--) {
                            Display::SendPixel(bmp->Pixel(x + y * bmp->Width()) ? pColor : pBgColor);
                        }
                    }
                } else {
                    int index = 0;
                    for (register int x = 0; x < bmp->Width(); x++) {
                        for (register int y = 0; y < bmp->Height(); y++) {
                            Display::SendPixel(bmp->Pixel(index++) ? pColor : pBgColor);
                        }
                    }
                }
            }
        }

        template<class BitmapType>
        static void DrawBitmapOverlay(int x1, int y1, const BitmapType &bmp, uint32_t color) {
            int index    = 0;
            uint16_t col = Display::ToPixelColor(color);
            for (register int x = x1; x < x1 + bmp.Width(); x++) {
                for (register int y = y1; y < y1 + bmp.Height(); y++) {
                    if (bmp[index]) {
                        DrawPixel_RawColor(x, y, col);
                    }
                    index++;
                }
            }
        }

        static void DrawImage(int x1, int y1, int x2, int y2, uint16_t *src) {
            Display::SetRegion(x1, y1, x2, y2);

            auto count = (x2 - x1 + 1) * (y2 - y1 + 1);
            for (int i = 0; i < count; i++) {
                Display::SendPixel(*src++);
            }
        }

        static void DrawMonoImage(const Vector2<int> pos, const Vector2<int> size, uint8_t *data, size_t bgColor, size_t fgColor) {
            Display::SetRegion(pos.X(), pos.Y(), pos.X() + size.X() - 1, pos.Y() + size.Y() - 1);

            int bgr    = bgColor & 0xFF;
            int bgg    = (bgColor >> 8) & 0xFF;
            int bgb    = (bgColor >> 16) & 0xFF;
            int fgr    = fgColor & 0xFF;
            int fgg    = (fgColor >> 8) & 0xFF;
            int fgb    = (fgColor >> 16) & 0xFF;
            auto count = size.X() * size.Y();
            for (int i = 0; i < count; i++) {
                auto d = *data++;
                Display::SendPixel(Display::ToPixelColor(GFX::RGB(
                    CFXS::Math::Map(d, 0, 255, bgr, fgr), CFXS::Math::Map(d, 0, 255, bgg, fgg), CFXS::Math::Map(d, 0, 255, bgb, fgb))));
            }
        }

        template<class ImageType>
        static void DrawImage(int x1, int y1, const ImageType &img) {
            Display::SetRegion(x1, y1, x1 + img.Width() - 1, y1 + img.Height() - 1);

            int index = 0;
            int count = img.Width() * img.Height();
            auto data = (uint16_t *)img.Data();
            for (int i = 0; i < count; i++) {
                Display::SendPixel(*data++);
            }
        }

        template<class RectClass>
        static void DrawRect(const RectClass &rect, uint32_t color) {
            int x1 = rect.Position().X();
            int y1 = rect.Position().Y();
            int x2 = rect.Position().X() + rect.Size().X() - 1;
            int y2 = rect.Position().Y() + rect.Size().Y() - 1;

            Display::SetRegion(x1, y1, x2, y2);
            int count    = ((x2 - x1) + 1) * ((y2 - y1) + 1);
            uint16_t col = Display::ToPixelColor(color);
            for (int i = 0; i < count; i++) {
                Display::SendPixel(col);
            }
        }

        static void DrawRect(Rect2i &&rect, uint32_t color) {
            int x1 = rect.Position().X();
            int y1 = rect.Position().Y();
            int x2 = rect.Position().X() + rect.Size().X() - 1;
            int y2 = rect.Position().Y() + rect.Size().Y() - 1;

            Display::SetRegion(x1, y1, x2, y2);
            int count    = ((x2 - x1) + 1) * ((y2 - y1) + 1);
            uint16_t col = Display::ToPixelColor(color);
            for (int i = 0; i < count; i++) {
                Display::SendPixel(col);
            }
        }

        template<class RectClass>
        static void DrawOutline(const RectClass &rect, uint8_t thickness, uint32_t color) {
            int x1 = rect.Position().X();
            int y1 = rect.Position().Y();
            int w  = rect.Size().X() - thickness;
            int h  = rect.Size().Y() - thickness;

            DrawRect({{x1, y1}, {w, thickness}}, color);
            DrawRect({{x1, y1 + h}, {w + thickness, thickness}}, color);
            DrawRect({{x1, y1}, {thickness, h}}, color);
            DrawRect({{x1 + w, y1}, {thickness, h}}, color);
        }

        template<class RectClass>
        static void DrawRectWithOutline(const RectClass &rect, uint8_t edges, uint8_t thickness, uint32_t color, uint32_t outlineColor) {
            if (!rect.Size().X() || !rect.Size().Y())
                return;

            int x1 = rect.Position().X();
            int y1 = rect.Position().Y();
            int x2 = rect.Position().X() + rect.Size().X() - 1;
            int y2 = rect.Position().Y() + rect.Size().Y() - 1;

            SWAP_IF_BIGGER(x1, x2);
            SWAP_IF_BIGGER(y1, y2);

            Display::SetRegion(x1, y1, x2, y2);

            uint16_t conv_color        = Display::ToPixelColor(color);
            uint16_t conv_outlineColor = Display::ToPixelColor(outlineColor);

            for (int y = y1; y <= y2; y++) {
                bool topCheck    = y < (y1 + thickness);
                bool bottomCheck = y > (y2 - thickness);
                for (int x = x1; x <= x2; x++) {
                    bool leftCheck  = x < (x1 + thickness);
                    bool rightCheck = x > (x2 - thickness);
                    bool endCheck   = false;

                    if (edges & (uint32_t)GFX::Edges::TOP) {
                        endCheck |= topCheck;
                    }
                    if (edges & (uint32_t)GFX::Edges::BOTTOM) {
                        endCheck |= bottomCheck;
                    }
                    if (edges & (uint32_t)GFX::Edges::LEFT) {
                        endCheck |= leftCheck;
                    }
                    if (edges & (uint32_t)GFX::Edges::RIGHT) {
                        endCheck |= rightCheck;
                    }

                    Display::SendPixel(endCheck ? conv_outlineColor : conv_color);
                }
            }
        }

        template<class RectClass>
        static void DrawBar(const RectClass &rect, int value, uint32_t barColor, uint32_t highlightColor, int valueRange = 100) {
            if (!rect.Size().X() || !rect.Size().Y())
                return;

            int x1 = rect.Position().X();
            int y1 = rect.Position().Y();
            int x2 = rect.Position().X() + rect.Size().X() - 1;
            int y2 = rect.Position().Y() + rect.Size().Y() - 1;

            SWAP_IF_BIGGER(x1, x2);
            SWAP_IF_BIGGER(y1, y2);

            Display::SetRegion(x1, y1, x2, y2);

            uint16_t conv_highlightColor = Display::ToPixelColor(highlightColor);
            uint16_t conv_barColor       = Display::ToPixelColor(barColor);

            int thickness = CFXS::Math::Map(value, 0, valueRange, 0, rect.Size().Y());
            for (int y = y1; y <= y2; y++) {
                bool bottomCheck = y > (y2 - thickness);
                for (int x = x1; x <= x2; x++) {
                    Display::SendPixel(bottomCheck ? conv_highlightColor : conv_barColor);
                }
            }
        }

        template<class RectClass>
        static void DrawRectWithOutline_ProgressBar(const RectClass &rect,
                                                    uint8_t thickness,
                                                    uint32_t bgColor,
                                                    uint32_t barColor,
                                                    uint32_t outlineColor,
                                                    uint32_t value,
                                                    uint32_t maxValue) {
            Rect<typename RectClass::TypeName> localRect = rect;

            DrawOutline(rect, thickness, outlineColor);

            localRect.Position() += Vector2<typename RectClass::TypeName>(thickness, thickness);
            localRect.Size() -= Vector2<typename RectClass::TypeName>(thickness * 2, thickness * 2);

            int origWidth = localRect.Size().X();
            int width     = CFXS::Math::Map(value, 0, maxValue, 0, origWidth);
            localRect.Size().SetX(width);

            DrawRect(localRect, barColor);

            localRect.Size().SetX((origWidth - width) - 1);
            localRect.Position().SetX(localRect.Position().X() + width + 1);

            DrawRect(localRect, bgColor);
        }
    };

} // namespace UI
#undef SWAP_IF_BIGGER
