#pragma once

#include <Utils/_GFXUtils.hpp>
#include <EASTL/functional.h>

namespace GFX {

    using Color = size_t;

    ////////////////////////////////////////////////////////////////////////////////

    enum class Axis { X, Y, HORIZONTAL = X, VERTICAL = Y };

    ////////////////////////////////////////////////////////////////////////////////

    enum TextAlignment {
        LEFT          = 0x01,
        RIGHT         = 0x02,
        TOP           = 0x04,
        BOTTOM        = 0x08,
        CENTER_H      = 0x10,
        CENTER_V      = 0x20,
        CENTER        = CENTER_H | CENTER_V,
        CENTER_LEFT   = CENTER_V | LEFT,
        TOP_CENTER    = CENTER_H | TOP,
        BOTTOM_CENTER = CENTER_H | BOTTOM,
        TOP_LEFT      = TOP | LEFT,
        BOTTOM_LEFT   = BOTTOM | LEFT,
        BOTTOM_RIGHT  = BOTTOM | RIGHT,
        TOP_RIGHT     = TOP | RIGHT
    };

    ////////////////////////////////////////////////////////////////////////////////

    namespace Edges {
        enum Edges { TOP = 0x01, BOTTOM = 0x02, RIGHT = 0x04, LEFT = 0x08, ALL = 0x0F };
    }

    ////////////////////////////////////////////////////////////////////////////////

    static constexpr uint32_t RGB(uint8_t red, uint8_t green, uint8_t blue) {
        return (red << 16) | (green << 8) | blue;
    }

    static constexpr uint32_t RGB(uint8_t val) {
        return (val << 16) | (val << 8) | val;
    }

    static uint32_t HSV(uint8_t h, uint8_t s, uint8_t v) {
        HsvColor ih  = {h, s, v};
        RgbColor rgb = HsvToRgb(ih);
        return GFX::RGB((int)rgb.r, (int)rgb.g, (int)rgb.b);
    }

    static constexpr uint32_t ce_Blend565(uint32_t color1, uint32_t color2, uint8_t alpha) {
        if (alpha == 0) {
            return color1;
        } else if (alpha == 255) {
            return color2;
        }

        uint32_t rb = color1 & 0xff00ff;
        uint32_t g  = color1 & 0x00ff00;
        rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
        g += ((color2 & 0x00ff00) - g) * alpha >> 8;
        return (rb & 0xff00ff) | (g & 0xff00);
    }

    static constexpr uint32_t Blend565(uint32_t color1, uint32_t color2, uint8_t alpha) {
        if (alpha == 0) {
            return color1;
        } else if (alpha == 255) {
            return color2;
        }

        uint32_t rb = color1 & 0xff00ff;
        uint32_t g  = color1 & 0x00ff00;
        rb += ((color2 & 0xff00ff) - rb) * alpha >> 8;
        g += ((color2 & 0x00ff00) - g) * alpha >> 8;
        return (rb & 0xff00ff) | (g & 0xff00);
    }

    extern void DrawLine(int x0, int y0, int x1, int y1, eastl::function<void(int x, int y)> pixelCallback);

} // namespace GFX
