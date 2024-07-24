#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

struct Vector2D {
    float x, y;
    Vector2D() : x(0), y(0) {}
    Vector2D(float x, float y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& v) {
        return Vector2D(x + v.x, y + v.y);
    }

    Vector2D operator-(const Vector2D& v) {
        return Vector2D(x - v.x, y - v.y);
    }

    Vector2D operator*(float scalar) {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D operator/(float scalar) {
        if (scalar == 0) { 
            return Vector2D(0,0);
        }
        return Vector2D(x / scalar, y / scalar);
    }

    Vector2D operator=(const Vector2D& v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    Vector2D operator+=(const Vector2D& v) {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2D operator-=(const Vector2D& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2D operator*=(const Vector2D& v) {
        x *= v.x;
        y *= v.y;
        return *this;
    }

    float dotProduct(const Vector2D& v) {
        return (x * v.x + y * v.y);
    }

    float magnitude() {
        return std::sqrt(x * x + y * y);
    }

    Vector2D normalize() {
        float mag = magnitude();
        if (mag == 0) {
            return {0, 0};
        }
        return {x/mag, y/mag};
    }
};

#endif