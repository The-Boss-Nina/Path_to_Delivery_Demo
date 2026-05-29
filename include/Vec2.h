#pragma once

class Vec2 {
public:
    float x;
    float y;

    Vec2();
    Vec2(float x, float y);

    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(float scalar) const;

    float Magnitude() const;
    Vec2 Normalized() const;
    float Distance(const Vec2& other) const;
    float Inclination() const;
    Vec2 Rotated(float angle) const;
};
