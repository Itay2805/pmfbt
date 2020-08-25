#include <cmath>

#include "vector2.hpp"

vector2::vector2()
        : x(0.0f)
        , y(0.0f)
{}

vector2::vector2(float scalar)
        : x(scalar)
        , y(scalar)
{}

vector2::vector2(float x, float y)
        : x(x), y(y) {
}

vector2::vector2(const vector2& vector) {
    this->x = vector.x;
    this->y = vector.y;
}

vector2& vector2::add(const vector2& other) {
    x += other.x;
    y += other.y;

    return *this;
}

vector2& vector2::subtract(const vector2& other) {
    x -= other.x;
    y -= other.y;

    return *this;
}

vector2& vector2::multiply(const vector2& other) {
    x *= other.x;
    y *= other.y;

    return *this;
}

vector2& vector2::divide(const vector2& other) {
    x /= other.x;
    y /= other.y;

    return *this;
}

vector2& vector2::add(float value) {
    x += value;
    y += value;

    return *this;
}

vector2& vector2::subtract(float value) {
    x -= value;
    y -= value;

    return *this;
}

vector2& vector2::multiply(float value) {
    x *= value;
    y *= value;

    return *this;
}

vector2& vector2::divide(float value) {
    x /= value;
    y /= value;

    return *this;
}

vector2 operator+(vector2 left, const vector2& right) {
    return left.add(right);
}

vector2 operator-(vector2 left, const vector2& right) {
    return left.subtract(right);
}

vector2 operator*(vector2 left, const vector2& right) {
    return left.multiply(right);
}

vector2 operator/(vector2 left, const vector2& right) {
    return left.divide(right);
}

vector2 operator+(vector2 left, float value) {
    return vector2(left.x + value, left.y + value);
}

vector2 operator-(vector2 left, float value) {
    return vector2(left.x - value, left.y - value);
}

vector2 operator*(vector2 left, float value) {
    return vector2(left.x * value, left.y * value);
}

vector2 operator/(vector2 left, float value) {
    return vector2(left.x / value, left.y / value);
}

vector2& vector2::operator+=(const vector2& other) {
    return add(other);
}

vector2& vector2::operator-=(const vector2& other) {
    return subtract(other);
}

vector2& vector2::operator*=(const vector2& other) {
    return multiply(other);
}

vector2& vector2::operator/=(const vector2& other) {
    return divide(other);
}

vector2& vector2::operator+=(float value) {
    return add(value);
}

vector2& vector2::operator-=(float value) {
    return subtract(value);
}

vector2& vector2::operator*=(float value) {
    return multiply(value);
}

vector2& vector2::operator/=(float value) {
    return divide(value);
}

bool vector2::operator==(const vector2& other) const {
    return x == other.x && y == other.y;
}

bool vector2::operator!=(const vector2& other) const {
    return !(*this == other);
}

bool vector2::operator<(const vector2& other) const {
    return x < other.x && y < other.y;
}

bool vector2::operator<=(const vector2& other) const {
    return x <= other.x && y <= other.y;
}

bool vector2::operator>(const vector2& other) const {
    return x > other.x && y > other.y;
}

bool vector2::operator>=(const vector2& other) const {
    return x >= other.x && y >= other.y;
}

float vector2::distance(const vector2& other) const {
    float a = x - other.x;
    float b = y - other.y;
    return sqrt(a * a + b * b);
}

float vector2::dot(const vector2& other) const {
    return x * other.x + y * other.y;
}

float vector2::magnitude() const {
    return sqrt(x * x + y * y);
}

vector2 vector2::normalise() const {
    float length = magnitude();
    return vector2(x / length, y / length);
}
