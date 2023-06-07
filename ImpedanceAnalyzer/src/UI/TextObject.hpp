#pragma once

#include <UI/FontRenderer.hpp>

class TextObject {
public:
    TextObject(const Vector2<int16_t> pos, uint32_t textColor = 0xFFFFFF, uint32_t bgColor = 0) :
        m_Position(pos), m_TextColor(textColor), m_BackgroundColor(bgColor){};

    void DrawText(char* str, bool force = false) {
        if (strcmp(str, m_LastText) == 0 && !force)
            return;

        // clear last
        main_font.DrawString(m_Position, m_LastText, m_BackgroundColor, m_BackgroundColor);

        // draw current
        main_font.DrawString(m_Position, str, m_TextColor, m_BackgroundColor);

        // copy
        for (int i = 0; i < std::min<int>(sizeof(m_LastText) - 1, strlen(str)); i++) {
            m_LastText[i] = str[i];
        }
        m_LastText[sizeof(m_LastText) - 1] = 0;
    }

    void SetTextColor(uint32_t color) {
        m_TextColor = color;
    }

    void SetBackgroundColor(uint32_t color) {
        m_BackgroundColor = color;
    }

private:
    Vector2<int16_t> m_Position;
    uint32_t m_TextColor;
    uint32_t m_BackgroundColor;
    char m_LastText[64] = {};
};