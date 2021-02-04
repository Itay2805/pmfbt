#pragma once

struct vector2 {
    float x, y;

    vector2();
    vector2(float scalar);
    vector2(float x, float y);
    vector2(const vector2& vector);

    vector2& add(const vector2& other);
    vector2& subtract(const vector2& other);
    vector2& multiply(const vector2& other);
    vector2& divide(const vector2& other);

    vector2& add(float value);
    vector2& subtract(float value);
    vector2& multiply(float value);
    vector2& divide(float value);

    friend vector2 operator+(vector2 left, const vector2& right);
    friend vector2 operator-(vector2 left, const vector2& right);
    friend vector2 operator*(vector2 left, const vector2& right);
    friend vector2 operator/(vector2 left, const vector2& right);

    friend vector2 operator+(vector2 left, float value);
    friend vector2 operator-(vector2 left, float value);
    friend vector2 operator*(vector2 left, float value);
    friend vector2 operator/(vector2 left, float value);

    bool operator==(const vector2& other) const;
    bool operator!=(const vector2& other) const;

    vector2& operator+=(const vector2& other);
    vector2& operator-=(const vector2& other);
    vector2& operator*=(const vector2& other);
    vector2& operator/=(const vector2& other);

    vector2& operator+=(float value);
    vector2& operator-=(float value);
    vector2& operator*=(float value);
    vector2& operator/=(float value);

    bool operator<(const vector2& other) const;
    bool operator<=(const vector2& other) const;
    bool operator>(const vector2& other) const;
    bool operator>=(const vector2& other) const;

    float magnitude() const;
    vector2 normalise() const;
    float distance(const vector2& other) const;
    float dot(const vector2& other) const;
};

static inline vector2 middle(vector2 a, vector2 b) {
    return vector2((a.x + b.x) / 2, (a.y + b.y) / 2);
}
