#pragma once
class Vector2f;

class Vector3f
{
public:
    static const Vector3f ZERO;
    static const Vector3f UP;
    static const Vector3f RIGHT;
    static const Vector3f FORWARD;

    Vector3f() = default;
    Vector3f(const Vector3f &other);
    Vector3f(float val = 0.0f);
    Vector3f(float x, float y, float z);

    Vector3f &operator=(const Vector3f &other);

    Vector3f &operator+=(const Vector3f &other);
    Vector3f &operator-=(const Vector3f &other);
    Vector3f &operator*=(float scalar);
    Vector3f &operator/=(float scalar);

    const float &operator[](int i) const;
    float &operator[](int i);

    float &x();
    float &y();
    float &z();

    float x() const;
    float y() const;
    float z() const;

    Vector2f xy() const;
    Vector2f xz() const;
    Vector2f yz() const;

    Vector3f xyz() const;
    Vector3f yzx() const;
    Vector3f zxy() const;

    float Length() const;
    float LengthSquared() const;
    void Normalize();
    Vector3f Normalized() const;

    Vector2f Homogenized() const;

    void Negate();

    static float Distance(const Vector3f &a, const Vector3f &b);
    static Vector3f Lerp(const Vector3f &a, const Vector3f &b, float t);

private:
    float m_data[3];
};

// 逐分量
Vector3f operator+(const Vector3f &v0, const Vector3f &v1);
Vector3f operator-(const Vector3f &v0, const Vector3f &v1);
Vector3f operator&(const Vector3f &v0, const Vector3f &v1); // 逐分量乘法
Vector3f operator/(const Vector3f &v0, const Vector3f &v1); // 逐分量除法

float operator*(const Vector3f &v0, const Vector3f &v1);    // 点乘
Vector3f operator^(const Vector3f &v0, const Vector3f &v1); // 叉乘

Vector3f operator-(const Vector3f &v);

Vector3f operator*(float f, const Vector3f &v);
Vector3f operator*(const Vector3f &v, float f);
Vector3f operator/(const Vector3f &v, float f);

bool operator==(const Vector3f &v0, const Vector3f &v1);
bool operator!=(const Vector3f &v0, const Vector3f &v1);
