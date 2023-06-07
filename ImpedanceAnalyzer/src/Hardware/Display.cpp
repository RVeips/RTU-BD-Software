#include "Display.hpp"
#include <CFXS/HW/Drivers/Display/ST7789.hpp>
#include "_Display_HAL.hpp"
#include <CFXS/Platform/Benchmark/ExecutionTime.hpp>
#include <algorithm>

using LCD = CFXS::HW::ST7789<CFXS::HW::Interface_ST7899_4PIN<LCD_SPI, Pin_RST, Pin_DCSEL, 320, 170, 0, 35>>; // Adafruit 320x170

extern const uint8_t s_CFXS[];
static constexpr int CFXS_W = 100;
static constexpr int CFXS_H = 20;

void Display::Initialize() {
    LCD::Initialize();
    LCD::SetFrameRate(LCD::Base::FrameRate::_60);
    LCD::SetOrientation(true, true, false);

    // auto full_frame_cycles = CFXS::Benchmark::MeasureExecutionCycles([]() {
    LCD::ClearFrame16(0, 0, LCD::GetWidth(), LCD::GetHeight(), Display::ToPixelColor(0, 0, 0));
    // });
    // CFXS_printf("LCD clear time: %.1fms\n", full_frame_cycles / CFXS::CPU::GetCyclesPerMillisecond<float>());

    int offset_x = LCD::GetWidth() / 2 - CFXS_W / 2;
    int offset_y = LCD::GetHeight() / 2 - CFXS_H / 2;
    int idx      = 0;
    for (int y = offset_y; y < CFXS_H + offset_y; y++) {
        for (int x = offset_x; x < CFXS_W + offset_x; x++) {
            auto c = s_CFXS[idx++];
            LCD::SetPixel(x, y, Display::ToPixelColor(c, c, c));
        }
    }
    CFXS::CPU::BlockingMilliseconds(500);
}

void Display::SendPixel(uint16_t color) {
    LCD::SendPixel(color);
}

void Display::SetRegion(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD::SetRegion(x1, y1, x2, y2);
}

void Display::SetPixel(uint16_t x, uint16_t y, uint16_t color) {
    LCD::SetPixel(x, y, color);
}

void Display::Clear() {
    LCD::ClearFrame16(0, 0, LCD::GetWidth(), LCD::GetHeight(), 0);
}

void Display::Process() {
}