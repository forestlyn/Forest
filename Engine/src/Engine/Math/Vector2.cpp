// Math/Vector2.cpp
#include "Vector2.h"
#include <glm/vec2.hpp>

namespace Engine::Math
{

    // Pimpl 实现结构体
    struct Vector2::Impl
    {
        glm::vec2 data;

        Impl(float x, float y) : data(x, y) {}
        Impl(const glm::vec2 &vec) : data(vec) {}
    };

    // 构造函数
    Vector2::Vector2(float x, float y)
        : x(x), y(y), pImpl(std::make_unique<Impl>(x, y)) {}

    Vector2::Vector2(const Vector2 &other)
        : x(other.x), y(other.y), pImpl(std::make_unique<Impl>(*other.pImpl)) {}

    Vector2::Vector2(Vector2 &&other) noexcept
        : x(other.x), y(other.y), pImpl(std::move(other.pImpl)) {}

    Vector2::~Vector2() = default;

    // 赋值运算符
    Vector2 &Vector2::operator=(const Vector2 &other)
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            pImpl = std::make_unique<Impl>(*other.pImpl);
        }
        return *this;
    }

    Vector2 &Vector2::operator=(Vector2 &&other) noexcept
    {
        if (this != &other)
        {
            x = other.x;
            y = other.y;
            pImpl = std::move(other.pImpl);
        }
        return *this;
    }

    // 算术运算符
    Vector2 Vector2::operator+(const Vector2 &other) const
    {
        return Vector2(pImpl->data + other.pImpl->data);
    }

    Vector2 Vector2::operator-(const Vector2 &other) const
    {
        return Vector2(pImpl->data - other.pImpl->data);
    }

    Vector2 Vector2::operator*(float scalar) const
    {
        return Vector2(pImpl->data * scalar);
    }

    Vector2 Vector2::operator/(float scalar) const
    {
        if (std::abs(scalar) < std::numeric_limits<float>::epsilon())
        {
            // 可以抛出异常或返回零向量，这里返回零向量
            return Vector2::Zero();
        }
        return Vector2(pImpl->data / scalar);
    }

    // 复合赋值运算符
    Vector2 &Vector2::operator+=(const Vector2 &other)
    {
        pImpl->data += other.pImpl->data;
        x = pImpl->data.x;
        y = pImpl->data.y;
        return *this;
    }

    Vector2 &Vector2::operator-=(const Vector2 &other)
    {
        pImpl->data -= other.pImpl->data;
        x = pImpl->data.x;
        y = pImpl->data.y;
        return *this;
    }

    Vector2 &Vector2::operator*=(float scalar)
    {
        pImpl->data *= scalar;
        x = pImpl->data.x;
        y = pImpl->data.y;
        return *this;
    }

    Vector2 &Vector2::operator/=(float scalar)
    {
        if (std::abs(scalar) < std::numeric_limits<float>::epsilon())
        {
            // 处理除零情况
            pImpl->data = glm::vec2(0.0f);
            x = 0.0f;
            y = 0.0f;
        }
        else
        {
            pImpl->data /= scalar;
            x = pImpl->data.x;
            y = pImpl->data.y;
        }
        return *this;
    }

    // 比较运算符
    bool Vector2::operator==(const Vector2 &other) const
    {
        return pImpl->data == other.pImpl->data;
    }

    bool Vector2::operator!=(const Vector2 &other) const
    {
        return pImpl->data != other.pImpl->data;
    }

    // 一元运算符
    Vector2 Vector2::operator-() const
    {
        return Vector2(-pImpl->data);
    }

    // 向量运算
    float Vector2::Dot(const Vector2 &other) const
    {
        return glm::dot(pImpl->data, other.pImpl->data);
    }

    float Vector2::Cross(const Vector2 &other) const
    {
        return pImpl->data.x * other.pImpl->data.y - pImpl->data.y * other.pImpl->data.x;
    }

    // 长度相关
    float Vector2::Length() const
    {
        return glm::length(pImpl->data);
    }

    float Vector2::LengthSquared() const
    {
        return glm::dot(pImpl->data, pImpl->data);
    }

    float Vector2::Distance(const Vector2 &other) const
    {
        return glm::distance(pImpl->data, other.pImpl->data);
    }

    float Vector2::DistanceSquared(const Vector2 &other) const
    {
        glm::vec2 diff = pImpl->data - other.pImpl->data;
        return glm::dot(diff, diff);
    }

    // 归一化
    Vector2 Vector2::Normalized() const
    {
        float len = Length();
        if (len < std::numeric_limits<float>::epsilon())
        {
            return Vector2::Zero();
        }
        return Vector2(pImpl->data / len);
    }

    Vector2 &Vector2::Normalize()
    {
        float len = Length();
        if (len > std::numeric_limits<float>::epsilon())
        {
            pImpl->data /= len;
            x = pImpl->data.x;
            y = pImpl->data.y;
        }
        return *this;
    }

    // 插值
    Vector2 Vector2::Lerp(const Vector2 &a, const Vector2 &b, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return Vector2(glm::mix(a.pImpl->data, b.pImpl->data, t));
    }

    // 工具函数
    bool Vector2::IsZero() const
    {
        return LengthSquared() < std::numeric_limits<float>::epsilon();
    }

    bool Vector2::IsNormalized() const
    {
        float lenSq = LengthSquared();
        return std::abs(lenSq - 1.0f) < 1e-5f;
    }

    std::string Vector2::ToString() const
    {
        std::stringstream ss;
        ss << "Vector2(" << x << ", " << y << ")";
        return ss.str();
    }

    // 静态常量
    Vector2 Vector2::Zero() { return Vector2(0.0f, 0.0f); }
    Vector2 Vector2::One() { return Vector2(1.0f, 1.0f); }
    Vector2 Vector2::UnitX() { return Vector2(1.0f, 0.0f); }
    Vector2 Vector2::UnitY() { return Vector2(0.0f, 1.0f); }
    Vector2 Vector2::Left() { return Vector2(-1.0f, 0.0f); }
    Vector2 Vector2::Right() { return Vector2(1.0f, 0.0f); }
    Vector2 Vector2::Up() { return Vector2(0.0f, 1.0f); }
    Vector2 Vector2::Down() { return Vector2(0.0f, -1.0f); }

    // 友元函数
    Vector2 operator*(float scalar, const Vector2 &vec)
    {
        return vec * scalar;
    }

}