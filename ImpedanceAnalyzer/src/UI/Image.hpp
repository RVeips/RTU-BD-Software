#pragma once

namespace GFX {
#pragma pack(1)
    struct Image {
        uint16_t m_Width;
        uint16_t m_Height;
        uint16_t m_Data[];
        ///////////////
        constexpr uint16_t Width() const {
            return m_Width;
        }
        constexpr uint16_t Height() const {
            return m_Height;
        }
        constexpr const uint16_t* Data() const {
            return m_Data;
        }
        constexpr bool operator[](size_t index) const {
            return Data()[index];
        }
    };

#pragma pack(1)
    struct DynamicImage {
#pragma pack(1)
        struct {
            uint16_t m_Width;
            uint16_t m_Height;
        };
        uint16_t* m_Data;
        ///////////////
        constexpr void SetWidth(uint16_t width) {
            m_Width = width;
        }
        constexpr void SetHeight(uint16_t height) {
            m_Height = height;
        }
        constexpr uint16_t Width() const {
            return m_Width;
        }
        constexpr uint16_t Height() const {
            return m_Height;
        }
        constexpr uint16_t* Data() const {
            return m_Data;
        }
        constexpr void SetData(uint16_t* data) {
            m_Data = data;
        }
        constexpr void SetData(const uint16_t* data) {
            m_Data = const_cast<uint16_t*>(data);
        }
        constexpr bool operator[](size_t index) const {
            return Data()[index];
        }
    };
#pragma pack()
} // namespace GFX
