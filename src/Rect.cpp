#include "Rect.h"

Rect::Rect() : x(0), y(0), w(0), h(0) {}
Rect::Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

Rect Rect::operator+(const Vec2& vec) const {
    return Rect(x + vec.x, y + vec.y, w, h);
}

Vec2 Rect::Center() const {
    return Vec2(x + w / 2.0f, y + h / 2.0f);
}

float Rect::Distance(const Rect& other) const {
    return Center().Distance(other.Center());
}

bool Rect::Contains(const Vec2& point) const {
    return point.x >= x && point.x <= x + w && point.y >= y && point.y <= y + h;
}
