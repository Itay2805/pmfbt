#include <cmath>

#include "vector3.hpp"

vector3::vector3()
        : x(0.0f), y(0.0f), z(0.0f) {
}

vector3::vector3(float scalar)
        : x(scalar), y(scalar), z(scalar) {
}

vector3::vector3(float x, float y, float z)
        : x(x), y(y), z(z) {
}

vector3::vector3(float x, float y)
        : x(x), y(y), z(0.0f) {
}

vector3 vector3::up() {
    return vector3(0.0f, 1.0f, 0.0f);
}

vector3 vector3::down() {
    return vector3(0.0f, -1.0f, 0.0f);
}

vector3 vector3::left() {
    return vector3(-1.0f, 0.0f, 0.0f);
}

vector3 vector3::right() {
    return vector3(1.0f, 1.0f, 0.0f);
}

vector3 vector3::zero() {
    return vector3(0.0f, 0.0f, 0.0f);
}

vector3 vector3::xaxis() {
    return vector3(1.0f, 0.0f, 0.0f);
}

vector3 vector3::yaxis() {
    return vector3(0.0f, 1.0f, 0.0f);
}

vector3 vector3::zaxis() {
    return vector3(0.0f, 0.0f, 1.0f);
}

vector3& vector3::add(const vector3& other) {
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

vector3& vector3::subtract(const vector3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

vector3& vector3::multiply(const vector3& other) {
    x *= other.x;
    y *= other.y;
    z *= other.z;

    return *this;
}

vector3& vector3::divide(const vector3& other) {
    x /= other.x;
    y /= other.y;
    z /= other.z;

    return *this;
}

vector3& vector3::add(float other) {
    x += other;
    y += other;
    z += other;

    return *this;
}

vector3& vector3::subtract(float other) {
    x -= other;
    y -= other;
    z -= other;

    return *this;
}

vector3& vector3::multiply(float other) {
    x *= other;
    y *= other;
    z *= other;

    return *this;
}

vector3& vector3::divide(float other) {
    x /= other;
    y /= other;
    z /= other;

    return *this;
}

vector3 operator+(vector3 left, const vector3& right) {
    return left.add(right);
}

vector3 operator-(vector3 left, const vector3& right) {
    return left.subtract(right);
}

vector3 operator*(vector3 left, const vector3& right) {
    return left.multiply(right);
}

vector3 operator/(vector3 left, const vector3& right) {
    return left.divide(right);
}

vector3 operator+(vector3 left, float right) {
    return left.add(right);
}

vector3 operator-(vector3 left, float right) {
    return left.subtract(right);
}

vector3 operator*(vector3 left, float right) {
    return left.multiply(right);
}

vector3 operator/(vector3 left, float right) {
    return left.divide(right);
}

vector3& vector3::operator+=(const vector3& other) {
    return add(other);
}

vector3& vector3::operator-=(const vector3& other) {
    return subtract(other);
}

vector3& vector3::operator*=(const vector3& other) {
    return multiply(other);
}

vector3& vector3::operator/=(const vector3& other) {
    return divide(other);
}

vector3& vector3::operator+=(float other) {
    return add(other);
}

vector3& vector3::operator-=(float other) {
    return subtract(other);
}

vector3& vector3::operator*=(float other) {
    return multiply(other);
}

vector3& vector3::operator/=(float other) {
    return divide(other);
}

bool vector3::operator<(const vector3& other) const {
    return x < other.x && y < other.y && z < other.z;
}

bool vector3::operator<=(const vector3& other) const {
    return x <= other.x && y <= other.y && z <= other.z;
}

bool vector3::operator>(const vector3& other) const {
    return x > other.x && y > other.y && z > other.z;
}

bool vector3::operator>=(const vector3& other) const {
    return x >= other.x && y >= other.y && z >= other.z;
}

bool vector3::operator==(const vector3& other) const {
    return x == other.x && y == other.y && z == other.z;
}

bool vector3::operator!=(const vector3& other) const {
    return !(*this == other);
}

vector3 operator-(const vector3& vector) {
    return vector3(-vector.x, -vector.y, -vector.z);
}

vector3 vector3::cross(const vector3& other) const {
    return vector3(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x);
}

float vector3::dot(const vector3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

float vector3::magnitude() const {
    return sqrt(x * x + y * y + z * z);
}

vector3 vector3::normalize() const {
    float length = magnitude();
    return vector3(x / length, y / length, z / length);
}

float vector3::distance(const vector3& other) const {
    float a = x - other.x;
    float b = y - other.y;
    float c = z - other.z;
    return sqrt(a * a + b * b + c * c);
}

vector2 vector3::xy() {
    return vector2(x, y);
}
