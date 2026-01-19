#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include "Vector4f.h"
#include "Vector2f.h"
#include "Vector3f.h"

Vector4f::Vector4f(float f)
{
	m_data[0] = f;
	m_data[1] = f;
	m_data[2] = f;
	m_data[3] = f;
}

Vector4f::Vector4f(float fx, float fy, float fz, float fw)
{
	m_data[0] = fx;
	m_data[1] = fy;
	m_data[2] = fz;
	m_data[3] = fw;
}

Vector4f::Vector4f(float f[4])
{
	m_data[0] = f[0];
	m_data[1] = f[1];
	m_data[2] = f[2];
	m_data[3] = f[3];
}

Vector4f::Vector4f(const Vector2f &xy, float z, float w)
{
	m_data[0] = xy.x();
	m_data[1] = xy.y();
	m_data[2] = z;
	m_data[3] = w;
}

Vector4f::Vector4f(float x, const Vector2f &yz, float w)
{
	m_data[0] = x;
	m_data[1] = yz.x();
	m_data[2] = yz.y();
	m_data[3] = w;
}

Vector4f::Vector4f(float x, float y, const Vector2f &zw)
{
	m_data[0] = x;
	m_data[1] = y;
	m_data[2] = zw.x();
	m_data[3] = zw.y();
}

Vector4f::Vector4f(const Vector2f &xy, const Vector2f &zw)
{
	m_data[0] = xy.x();
	m_data[1] = xy.y();
	m_data[2] = zw.x();
	m_data[3] = zw.y();
}

Vector4f::Vector4f(const Vector3f &xyz, float w)
{
	m_data[0] = xyz.x();
	m_data[1] = xyz.y();
	m_data[2] = xyz.z();
	m_data[3] = w;
}

Vector4f::Vector4f(float x, const Vector3f &yzw)
{
	m_data[0] = x;
	m_data[1] = yzw.x();
	m_data[2] = yzw.y();
	m_data[3] = yzw.z();
}

Vector4f::Vector4f(const Vector4f &rv)
{
	m_data[0] = rv.m_data[0];
	m_data[1] = rv.m_data[1];
	m_data[2] = rv.m_data[2];
	m_data[3] = rv.m_data[3];
}

Vector4f &Vector4f::operator=(const Vector4f &rv)
{
	if (this != &rv)
	{
		m_data[0] = rv.m_data[0];
		m_data[1] = rv.m_data[1];
		m_data[2] = rv.m_data[2];
		m_data[3] = rv.m_data[3];
	}
	return *this;
}

const float &Vector4f::operator[](int i) const
{
	return m_data[i];
}

float &Vector4f::operator[](int i)
{
	return m_data[i];
}

float &Vector4f::x()
{
	return m_data[0];
}

float &Vector4f::y()
{
	return m_data[1];
}

float &Vector4f::z()
{
	return m_data[2];
}

float &Vector4f::w()
{
	return m_data[3];
}

float Vector4f::x() const
{
	return m_data[0];
}

float Vector4f::y() const
{
	return m_data[1];
}

float Vector4f::z() const
{
	return m_data[2];
}

float Vector4f::w() const
{
	return m_data[3];
}

Vector2f Vector4f::xy() const
{
	return Vector2f(m_data[0], m_data[1]);
}

Vector2f Vector4f::yz() const
{
	return Vector2f(m_data[1], m_data[2]);
}

Vector2f Vector4f::zw() const
{
	return Vector2f(m_data[2], m_data[3]);
}

Vector2f Vector4f::wx() const
{
	return Vector2f(m_data[3], m_data[0]);
}

Vector3f Vector4f::xyz() const
{
	return Vector3f(m_data[0], m_data[1], m_data[2]);
}

Vector3f Vector4f::yzw() const
{
	return Vector3f(m_data[1], m_data[2], m_data[3]);
}

Vector3f Vector4f::zwx() const
{
	return Vector3f(m_data[2], m_data[3], m_data[0]);
}

Vector3f Vector4f::wxy() const
{
	return Vector3f(m_data[3], m_data[0], m_data[1]);
}

Vector3f Vector4f::xyw() const
{
	return Vector3f(m_data[0], m_data[1], m_data[3]);
}

Vector3f Vector4f::yzx() const
{
	return Vector3f(m_data[1], m_data[2], m_data[0]);
}

Vector3f Vector4f::zwy() const
{
	return Vector3f(m_data[2], m_data[3], m_data[1]);
}

Vector3f Vector4f::wxz() const
{
	return Vector3f(m_data[3], m_data[0], m_data[2]);
}
float Vector4f::Length() const
{
	return sqrt(m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2] + m_data[3] * m_data[3]);
}

float Vector4f::LengthSquared() const
{
	return (m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2] + m_data[3] * m_data[3]);
}

void Vector4f::Normalize()
{
	float norm = sqrt(m_data[0] * m_data[0] + m_data[1] * m_data[1] + m_data[2] * m_data[2] + m_data[3] * m_data[3]);
	m_data[0] = m_data[0] / norm;
	m_data[1] = m_data[1] / norm;
	m_data[2] = m_data[2] / norm;
	m_data[3] = m_data[3] / norm;
}

Vector4f Vector4f::Normalized() const
{
	float length = Length();
	return Vector4f(
		m_data[0] / length,
		m_data[1] / length,
		m_data[2] / length,
		m_data[3] / length);
}

void Vector4f::Homogenize()
{
	if (m_data[3] != 0)
	{
		m_data[0] /= m_data[3];
		m_data[1] /= m_data[3];
		m_data[2] /= m_data[3];
		m_data[3] = 1;
	}
}

Vector4f Vector4f::Homogenized() const
{
	if (m_data[3] != 0)
	{
		return Vector4f(
			m_data[0] / m_data[3],
			m_data[1] / m_data[3],
			m_data[2] / m_data[3],
			1);
	}
	else
	{
		return Vector4f(
			m_data[0],
			m_data[1],
			m_data[2],
			m_data[3]);
	}
}

void Vector4f::Negate()
{
	m_data[0] = -m_data[0];
	m_data[1] = -m_data[1];
	m_data[2] = -m_data[2];
	m_data[3] = -m_data[3];
}



float Vector4f::Distance(const Vector4f &a, const Vector4f &b)
{
    return (a - b).Length();
}
Vector4f Vector4f::Lerp(const Vector4f &v0, const Vector4f &v1, float alpha)
{
	return alpha * (v1 - v0) + v0;
}

Vector4f operator+(const Vector4f &v0, const Vector4f &v1)
{
	return Vector4f(v0.x() + v1.x(), v0.y() + v1.y(), v0.z() + v1.z(), v0.w() + v1.w());
}

Vector4f operator-(const Vector4f &v0, const Vector4f &v1)
{
	return Vector4f(v0.x() - v1.x(), v0.y() - v1.y(), v0.z() - v1.z(), v0.w() - v1.w());
}

Vector4f operator&(const Vector4f &v0, const Vector4f &v1)
{
	return Vector4f(v0.x() * v1.x(), v0.y() * v1.y(), v0.z() * v1.z(), v0.w() * v1.w());
}

Vector4f operator/(const Vector4f &v0, const Vector4f &v1)
{
	return Vector4f(v0.x() / v1.x(), v0.y() / v1.y(), v0.z() / v1.z(), v0.w() / v1.w());
}

Vector4f operator-(const Vector4f &v)
{
	return Vector4f(-v.x(), -v.y(), -v.z(), -v.w());
}


float operator*(const Vector4f &v0, const Vector4f &v1)
{
	return v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z() + v0.w() * v1.w();
}

Vector4f operator*(float f, const Vector4f &v)
{
	return Vector4f(f * v.x(), f * v.y(), f * v.z(), f * v.w());
}

Vector4f operator*(const Vector4f &v, float f)
{
	return Vector4f(f * v.x(), f * v.y(), f * v.z(), f * v.w());
}

Vector4f operator/(const Vector4f &v, float f)
{
	return Vector4f(v[0] / f, v[1] / f, v[2] / f, v[3] / f);
}

bool operator==(const Vector4f &v0, const Vector4f &v1)
{
	return (v0.x() == v1.x() && v0.y() == v1.y() && v0.z() == v1.z() && v0.w() == v1.w());
}

bool operator!=(const Vector4f &v0, const Vector4f &v1)
{
	return !(v0 == v1);
}
