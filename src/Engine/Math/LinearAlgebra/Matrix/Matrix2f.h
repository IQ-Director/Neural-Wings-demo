#pragma once
#include <cstdio>

class Vector2f;
class Matrix2f
{
public:
    static Matrix2f ones();
    static Matrix2f identity();
    static Matrix2f rotation(float degrees);

    Matrix2f(float fill = 0.f);
    Matrix2f(float m00, float m01,
             float m10, float m11);

    // setColumns = true ==>  matrix to be [v0 v1]
    Matrix2f(const Vector2f &v0, const Vector2f &v1, bool setColumns = true);

    Matrix2f(const Matrix2f &rm);
    Matrix2f &operator=(const Matrix2f &rm);

    const float &operator()(int i, int j) const;
    float &operator()(int i, int j);

    Vector2f getRow(int i) const;
    void setRow(int i, const Vector2f &v);

    Vector2f getCol(int j) const;
    void setCol(int j, const Vector2f &v);

    float determinant();
    Matrix2f inverse(bool *pbIsSingular = NULL, float epsilon = 0.f);

    void transpose();
    Matrix2f transposed() const;

    static float determinant2x2(float m00, float m01,
                                float m10, float m11);

private:
// [0 2
//  1 3]
    float m_data[4];
};

Matrix2f operator*(float f, const Matrix2f &m);
Matrix2f operator*(const Matrix2f &m, float f);

Vector2f operator*(const Matrix2f &m, const Vector2f &v);

Matrix2f operator*(const Matrix2f &x, const Matrix2f &y);
