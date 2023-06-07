#pragma once

#include <UI/Vector2.hpp>

template<typename T>
class Rect;

#pragma pack(1)
template<typename T>
class Rect {
public:
    using TypeName = T;

    Rect() = default;

    constexpr Rect(const Vector2<T> &pos, const Vector2<T> &size) : m_Position(pos), m_Size(size) {
    }

    template<typename RectType>
    Rect(Rect<RectType> &other) {
        m_Position = other.Position();
        m_Size     = other.Size();
    }

    template<typename RectType>
    void operator=(const Rect<RectType> &other) {
        m_Position = other.Position();
        m_Size     = other.Size();
    }

    template<typename VectorType>
    Rect operator>>(Vector2<VectorType> &vec) {
        return Rect(m_Position + vec, m_Size);
    }

    template<typename RectType>
    Rect<RectType> Convert() const {
        return Rect<RectType>(Vector2<RectType>((RectType)Position().X(), (RectType)Position().Y()),
                              Vector2<RectType>((RectType)Size().X(), (RectType)Size().Y()));
    }

    bool operator!=(const Rect &other) {
        return (Position() != other.Position()) || (Size() != other.Size());
    }

    bool operator==(const Rect &other) {
        return (Position() == other.Position()) && (Size() == other.Size());
    }

    template<typename VectorType>
    void operator>>=(const Vector2<VectorType> &vec) {
        m_Position += vec;
    }

    template<typename VectorType>
    constexpr bool Contains(const Vector2<VectorType> &vec) const {
        return (vec >= TopLeft()) && (vec <= BottomRight());
    }

    constexpr bool Contains(const Rect &other) const {
        return Contains(other.TopLeft()) && Contains(other.BottomRight());
    }

    constexpr bool Overlaps(const Rect &other) const {
        bool topLeftOverlap     = Contains<TypeName>(other.Convert<TypeName>().TopLeft());
        bool bottomRightOverlap = Contains<TypeName>(other.Convert<TypeName>().BottomRight());
        bool topRightOverlap    = Contains<TypeName>(other.Convert<TypeName>().TopRight());
        bool bottomLeftOverlap  = Contains<TypeName>(other.Convert<TypeName>().BottomLeft());
        return topLeftOverlap || bottomRightOverlap || topRightOverlap || bottomLeftOverlap;
    }

    constexpr Vector2<T> TopLeft() const {
        return m_Position;
    }

    constexpr Vector2<T> TopRight() {
        return m_Position + Vector2<T>(m_Size.X(), static_cast<T>(0));
    }

    constexpr Vector2<T> BottomLeft() {
        return m_Position + Vector2<T>(static_cast<T>(0), m_Size.Y());
    }

    constexpr Vector2<T> BottomRight() const {
        return m_Position + m_Size;
    }

    constexpr Vector2<T> Middle() {
        return m_Position + (m_Size / 2);
    }

    constexpr Vector2<T> Top() {
        return m_Position + Vector2<T>(m_Size.X() / 2, static_cast<T>(0));
    }

    constexpr Vector2<T> Bottom() {
        return m_Position + Vector2<T>(m_Size.X() / 2, static_cast<T>(m_Size.Y()));
    }

    constexpr Vector2<T> Left() {
        return m_Position + Vector2<T>(static_cast<T>(0), static_cast<T>(m_Size.Y() / 2));
    }

    constexpr Vector2<T> Right() {
        return m_Position + Vector2<T>(static_cast<T>(m_Size.X()), static_cast<T>(m_Size.Y() / 2));
    }

    inline void print() const {
        NAFT_printf("D{%d, %d, %d, %d}\n", m_Position.X(), m_Position.Y(), m_Size.X(), m_Size.Y());
    }

    ~Rect() = default;

public:
    Vector2<T> &Position() {
        return m_Position;
    }
    Vector2<T> &Size() {
        return m_Size;
    }
    const Vector2<T> &Position() const {
        return m_Position;
    }
    const Vector2<T> &Size() const {
        return m_Size;
    }
    void SetPosition(const Vector2<T> &pos) {
        m_Position = pos;
    }
    void SetSize(const Vector2<T> &size) {
        m_Size = size;
    }

private:
    Vector2<T> m_Position;
    Vector2<T> m_Size;
};
#pragma pack()

using Rect2i = Rect<int>;
