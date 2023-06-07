#pragma once

#pragma pack(1)
template<typename T>
class Padding {
public:
    Padding() = default;

    constexpr Padding(T top, T bottom, T left, T right) : m_Top(top), m_Bottom(bottom), m_Left(left), m_Right(right) {
    }

    Padding(const Padding &other) {
        m_Top    = other.m_Top;
        m_Bottom = other.m_Bottom;
        m_Left   = other.m_Left;
        m_Right  = other.m_Right;
    }

    void operator=(const Padding &other) {
        m_Top    = other.m_Top;
        m_Bottom = other.m_Bottom;
        m_Left   = other.m_Left;
        m_Right  = other.m_Right;
    }

    ~Padding() = default;

public:
    const T Top() const {
        return m_Top;
    }
    const T Bottom() const {
        return m_Bottom;
    }
    const T Left() const {
        return m_Left;
    }
    const T Right() const {
        return m_Right;
    }

    void SetTop(T top) {
        m_Top = top;
    }
    void SetBottom(T bottom) {
        m_Bottom = bottom;
    }
    void SetLeft(T left) {
        m_Left = left;
    }
    void SetRight(T right) {
        m_Right = right;
    }

private:
    T m_Top;
    T m_Bottom;
    T m_Left;
    T m_Right;
};
#pragma pack()
