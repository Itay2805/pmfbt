#pragma once

#include "vector2.hpp"

struct vector3 {
    float x, y, z;

    vector3();
    vector3(float scalar);
    vector3(float x, float y, float z);
    vector3(float x, float y);

    static vector3 up();
    static vector3 down();
    static vector3 left();
    static vector3 right();
    static vector3 zero();

    static vector3 xaxis();
    static vector3 yaxis();
    static vector3 zaxis();

    vector3& add(const vector3& other);
    vector3& subtract(const vector3& other);
    vector3& multiply(const vector3& other);
    vector3& divide(const vector3& other);

    vector3& add(float other);
    vector3& subtract(float other);
    vector3& multiply(float other);
    vector3& divide(float other);

    friend vector3 operator+(vector3 left, const vector3& right);
    friend vector3 operator-(vector3 left, const vector3& right);
    friend vector3 operator*(vector3 left, const vector3& right);
    friend vector3 operator/(vector3 left, const vector3& right);

    friend vector3 operator+(vector3 left, float right);
    friend vector3 operator-(vector3 left, float right);
    friend vector3 operator*(vector3 left, float right);
    friend vector3 operator/(vector3 left, float right);

    bool operator==(const vector3& other) const;
    bool operator!=(const vector3& other) const;

    vector3& operator+=(const vector3& other);
    vector3& operator-=(const vector3& other);
    vector3& operator*=(const vector3& other);
    vector3& operator/=(const vector3& other);

    vector3& operator+=(float other);
    vector3& operator-=(float other);
    vector3& operator*=(float other);
    vector3& operator/=(float other);

    bool operator<(const vector3& other) const;
    bool operator<=(const vector3& other) const;
    bool operator>(const vector3& other) const;
    bool operator>=(const vector3& other) const;

    friend vector3 operator-(const vector3& vector);

    vector3 cross(const vector3& other) const;
    float dot(const vector3& other) const;

    float magnitude() const;
    vector3 normalize() const;
    float distance(const vector3& other) const;

    vector2 xy();
};

static inline vector3 middle(vector3 a, vector3 b) {
    return vector3((a.x + b.x) / 2, (a.y + b.y) / 2, (a.z + b.z) / 2);
}
