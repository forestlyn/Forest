#pragma once
#include "Engine/pcheader.h"

namespace Engine::Math
{
    class Vector2
    {
    public:
        float x, y;

        // 构造函数
        Vector2(float x = 0.0f, float y = 0.0f);
        Vector2(const Vector2 &other);
        Vector2(Vector2 &&other) noexcept;
        ~Vector2();

        // 赋值运算符
        Vector2 &operator=(const Vector2 &other);
        Vector2 &operator=(Vector2 &&other) noexcept;

        // 算术运算符
        Vector2 operator+(const Vector2 &other) const;
        Vector2 operator-(const Vector2 &other) const;
        Vector2 operator*(float scalar) const;
        Vector2 operator/(float scalar) const;

        // 复合赋值运算符
        Vector2 &operator+=(const Vector2 &other);
        Vector2 &operator-=(const Vector2 &other);
        Vector2 &operator*=(float scalar);
        Vector2 &operator/=(float scalar);

        // 比较运算符
        bool operator==(const Vector2 &other) const;
        bool operator!=(const Vector2 &other) const;

        // 一元运算符
        Vector2 operator-() const;

        // 向量运算
        float Dot(const Vector2 &other) const;
        float Cross(const Vector2 &other) const;

        // 长度相关
        float Length() const;
        float LengthSquared() const;
        float Distance(const Vector2 &other) const;
        float DistanceSquared(const Vector2 &other) const;

        // 归一化
        Vector2 Normalized() const;
        Vector2 &Normalize();

        // 插值
        static Vector2 Lerp(const Vector2 &a, const Vector2 &b, float t);

        // 工具函数
        bool IsZero() const;
        bool IsNormalized() const;
        std::string ToString() const;

        // 静态常量
        static Vector2 Zero();
        static Vector2 One();
        static Vector2 UnitX();
        static Vector2 UnitY();
        static Vector2 Left();
        static Vector2 Right();
        static Vector2 Up();
        static Vector2 Down();

    private:
        // Pimpl 实现
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };

    // 标量乘法的友元函数（支持 float * Vector2 形式）
    Vector2 operator*(float scalar, const Vector2 &vec);
}