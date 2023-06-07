#pragma once

#include <UI/Padding.hpp>
#include <Utils/Math.hpp>
#include <cstdlib>

#pragma pack(1)
template<typename T>
class Vector2 {
public:
    Vector2() : m_X(0), m_Y(0) {
    }

    Vector2(T x) : m_X(x), m_Y(x) {
    }

    template<typename VectorType>
    Vector2<VectorType> Convert() const {
        return Vector2<VectorType>((VectorType)m_X, (VectorType)m_Y);
    }

    template<class TParam, class TParam2>
    constexpr Vector2(TParam x, TParam2 y) : m_X(static_cast<T>(x)), m_Y(static_cast<T>(y)) {
    }

    template<typename VectorType>
    Vector2(Vector2<VectorType> &other) {
        m_X = static_cast<T>(other.X());
        m_Y = static_cast<T>(other.Y());
    }

    template<typename VectorType>
    Vector2 &operator=(const Vector2<VectorType> &other) {
        m_X = other.X();
        m_Y = other.Y();
        return *this;
    }

    bool operator==(const Vector2 &other) {
        return (m_X == other.m_X) || (m_Y == other.m_Y);
    }

    bool operator!=(const Vector2 &other) {
        return (m_X != other.m_X) && (m_Y != other.m_Y);
    }

    template<typename VectorType>
    bool operator>(const Vector2<VectorType> &other) const {
        return (m_X > other.X()) && (m_Y > other.Y());
    }

    template<typename VectorType>
    bool operator<(const Vector2<VectorType> &other) const {
        return (m_X < other.X()) && (m_Y < other.Y());
    }

    template<typename VectorType>
    bool operator>=(const Vector2<VectorType> &other) const {
        return (m_X >= other.X()) && (m_Y >= other.Y());
    }

    template<typename VectorType>
    bool operator<=(const Vector2<VectorType> &other) const {
        return (m_X <= other.X()) && (m_Y <= other.Y());
    }

    template<typename VectorType>
    Vector2 operator+(const Vector2<VectorType> &other) const {
        return Vector2(m_X + other.X(), m_Y + other.Y());
    }

    Vector2 operator-(const Vector2 &other) {
        return Vector2(m_X - other.m_X, m_Y - other.m_Y);
    }

    Vector2 operator*(const Vector2 &other) {
        return Vector2(m_X * other.m_X, m_Y * other.m_Y);
    }

    Vector2 operator/(int val) {
        return Vector2(m_X / val, m_Y / val);
    }

    Vector2 operator/(const Vector2 &other) {
        return Vector2(m_X / other.m_X, m_Y / other.m_Y);
    }

    template<typename VectorType>
    void operator+=(const Vector2<VectorType> &other) {
        m_X += static_cast<T>(other.X());
        m_Y += static_cast<T>(other.Y());
    }

    void operator-=(const Vector2 &other) {
        m_X -= other.m_X;
        m_Y -= other.m_Y;
    }

    void operator*=(const Vector2 &other) {
        m_X *= other.m_X;
        m_Y *= other.m_Y;
    }

    void operator/=(const Vector2 &other) {
        m_X /= other.m_X;
        m_Y /= other.m_Y;
    }

    template<class PaddingType>
    Vector2 operator-(const Padding<PaddingType> &padding) {
        return Vector2(m_X - static_cast<T>(padding.Left() + padding.Right()), m_Y - static_cast<T>(padding.Top() + padding.Bottom()));
    }

    inline void print() const {
        NAFT_printf("Vector2[%p] (%d, %d)\n", this, m_X, m_Y);
    }

    ~Vector2() = default;

public:
    template<class TParam>
    constexpr void SetX(TParam x) {
        m_X = static_cast<T>(x);
    }

    template<class TParam>
    constexpr void SetY(TParam y) {
        m_Y = static_cast<T>(y);
    }

    constexpr T X() const {
        return m_X;
    }
    constexpr T Y() const {
        return m_Y;
    }
    constexpr T XY() const {
        return m_X * m_Y;
    }

    template<class TParam, class TParam2>
    constexpr void Set(TParam x, TParam2 y) {
        m_X = static_cast<T>(x);
        m_Y = static_cast<T>(y);
    }

    template<typename VectorType>
    constexpr T DistanceTo_NoRoot(const Vector2<VectorType> &other) {
        return std::abs(Utils::Math::Square(m_X - other.X()) + Utils::Math::Square(m_Y - other.Y()));
    }

private:
    T m_X;
    T m_Y;
};
#pragma pack()
