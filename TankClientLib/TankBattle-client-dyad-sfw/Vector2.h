#pragma once
#include <cmath>
#include <cassert>
#include <cfloat>
#define RADTODEG 57.2958f
struct Vector2
{
	union
	{
		struct { float x, y; };
		float v[2];
	};
	Vector2() {}
	Vector2(float a_x, float a_y)
		: x(a_x), y(a_y) {}

	float magnitude() const { return sqrtf(x*x + y*y); }

	float angle() const { return atan2f(y, x); }
	static Vector2 random();
	float &operator[](unsigned idx) { return v[idx]; }
	float  operator[](unsigned idx) const { return v[idx]; }
	static Vector2 fromXZ(float *vec3) { return Vector2{ vec3[0],vec3[2] }; }
	static Vector2 fromAngle(float a)
	{
		return{ cosf(a), sinf(a) };
	}

	float getAngleBetween(Vector2 a)
	{
		return atan2(a.y - y, a.x - x) * RADTODEG;
	}
};

// Direction Between Vectors:
//(A-B).normal()
// Distance Between Vectors:
//(A-B).magnitude()

inline Vector2 operator-(const Vector2  &lhs, const Vector2 &rhs)
{
	return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline Vector2 perp(const Vector2  &a)
{
	return Vector2(-a.y, a.x);
}


inline Vector2 operator+(const Vector2  &lhs, const Vector2 &rhs)
{
	return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Vector2 operator/(const Vector2 &lhs, float rhs)
{
	return Vector2(lhs.x / rhs, lhs.y / rhs);
}
inline Vector2 operator*(const Vector2 &a, const Vector2 &b) { return Vector2{ a.x * b.x, a.y * b.y }; }
inline Vector2 operator*(const Vector2 &a, float b) { return Vector2{ a.x * b  , a.y * b }; }
inline Vector2 operator*(float b, const Vector2 &a) { return Vector2{ a.x * b  , a.y * b }; }
// Dot production tells us how much one vector extends
// in the direction of another vector
inline float dot(const Vector2 &lhs, const Vector2 &rhs)
{
	return lhs.x * rhs.x + rhs.y * lhs.y;
}

//#define EPSILON 0.0001f

inline bool operator==(const Vector2 &lhs, const Vector2 &rhs)
{
	// return lhs.x == rhs.x && lhs.y == rhs.y;
	return
		rhs.x - FLT_EPSILON < lhs.x && lhs.x < rhs.x + FLT_EPSILON &&
		rhs.y - FLT_EPSILON < lhs.y && lhs.y < rhs.y + FLT_EPSILON;

	//return fabsf(rhs.x - lhs.x) < FLT_EPSILON &&
	//       fabsf(rhs.y - lhs.y) < FLT_EPSILON;
}

inline Vector2 normal(const Vector2 &a)
{
	//assert(magnitude() != 0 && "Divide by Zero");
	return a / a.magnitude();
}


inline bool operator<(const Vector2 &a, const Vector2 &b) { return a.x < b.x && a.y < b.y; }
inline bool operator>(const Vector2 &a, const Vector2 &b) { return a.x > b.x && a.y > b.y; }

inline Vector2 min(const Vector2 &a, const Vector2 &b) { return{ std::fminf(a.x, b.x), std::fminf(a.y, b.y) }; }
inline Vector2 max(const Vector2 &a, const Vector2 &b) { return{ std::fmaxf(a.x, b.x), std::fminf(a.y, b.y) }; }


inline Vector2 clamp(const Vector2 &val, const Vector2 &lower, const Vector2 &upper)
{
	return max(min(val, upper), lower);
}


// Affix a value to the lower and upper bounds
// Unlike clamping, this will move objects inside of the volume to the edges
inline Vector2 snap(const Vector2 &val, const Vector2 &lower, const Vector2 &upper)
{
	return{ val.x - lower.x < upper.x - val.x ? lower.x : upper.x,
		val.y - lower.y < upper.y - val.y ? lower.y : upper.y };
}


inline float distance(const Vector2 &a, const Vector2 &b)
{
	return sqrtf((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

// there is no 2D cross product, this is actually the perpendicular dot product.
// negative values mean b is to the "left" of a and positive values mean b is to the "right" of a.
inline float cross(const Vector2 &a, const Vector2 &b) { return dot(a, perp(b)); }

Vector2 Vector2::random()
{
	return normal(Vector2{ 1 - 2 * (float)rand() / RAND_MAX, 1 - 2 * (float)rand() / RAND_MAX });
}