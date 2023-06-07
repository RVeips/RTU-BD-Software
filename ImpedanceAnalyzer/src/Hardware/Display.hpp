#pragma once

class Display {
public:
    static void Initialize();

    static void SendPixel(uint16_t color);

    static void SetRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

    static void SetPixel(uint16_t x, uint16_t y, uint16_t color);

    static void Clear();

    static constexpr uint16_t ToPixelColor(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
    }

    static constexpr uint16_t ToPixelColor(uint32_t rgba) {
        uint8_t r = (rgba)&0xFF;
        uint8_t g = (rgba >> 8) & 0xFF;
        uint8_t b = (rgba >> 16) & 0xFF;
        return ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);
    }

    static int GetWidth() {
        return 320;
    }

    static int GetHeight() {
        return 170;
    }

private:
    static void Process();
};