#ifndef VECTOR_H
#define VECTOR_H

#include <pmmintrin.h>

_declspec(align(16))
struct Vector4 
{
public:
	union 
	{
		struct
		{
			float x, y, z, w;
		};
		float coords[4];
		__m128 SIMDCoords;
	};

	Vector4(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f) : x(_x), y(_y), z(_z), w(_w) {}
	Vector4(__m128 _SIMDCoords) : SIMDCoords(_SIMDCoords) {}

	Vector4 operator+(const __m128 &other) 
	{
		return Vector4(_mm_add_ps(SIMDCoords, other));
	}

	Vector4 operator-(const __m128 &other) 
	{
		return Vector4(_mm_sub_ps(SIMDCoords, other));
	}

	Vector4 operator*(const __m128 &other) 
	{
		return Vector4(_mm_mul_ps(SIMDCoords, other));
	}

	Vector4 operator/(const __m128 &other) 
	{
		return Vector4(_mm_div_ps(SIMDCoords, other));
	}

	Vector4 operator+(const Vector4 &other) 
	{
		return *this + other.SIMDCoords;
	}

	Vector4 operator-(const Vector4 &other) 
	{
		return *this - other.SIMDCoords;
	}

	Vector4 operator*(const Vector4 &other) 
	{
		return *this * other.SIMDCoords;
	}
	
	Vector4 operator/(const Vector4 &other)
	{
		return *this / other.SIMDCoords;
	}

	Vector4 operator+(float other) 
	{
		return Vector4(_mm_add_ps(SIMDCoords, _mm_set1_ps(other)));
	}

	Vector4 operator-(float other) 
	{
		return Vector4(_mm_sub_ps(SIMDCoords, _mm_set1_ps(other)));
	}

	Vector4 operator*(float other)
	{
		return Vector4(_mm_mul_ps(SIMDCoords, _mm_set1_ps(other)));
	}

	Vector4 operator/(float other)
	{
		return Vector4(_mm_div_ps(SIMDCoords, _mm_set1_ps(other)));
	}

	float length() 
	{
		//Cube the elements
		__m128 length = _mm_mul_ps(SIMDCoords, SIMDCoords);
		//Sum the elements
		length = _mm_hadd_ps(length,length);
		length = _mm_hadd_ps(length,length);
		//Square root of the elements
		length = _mm_sqrt_ps(length);
		//Store in float array and return first element
		float result[4];
		_mm_storeu_ps(result, length);
		return result[0];
	}

	Vector4& normalize() 
	{
		//Cube the elements
		__m128 denominator = _mm_mul_ps(SIMDCoords, SIMDCoords);
		//Sum the elements
		denominator = _mm_hadd_ps(denominator,denominator);
		denominator = _mm_hadd_ps(denominator,denominator);
		//Square root of the elements
		denominator = _mm_sqrt_ps(denominator);

		return (*this)/denominator;
	}
};

typedef Vector4 vec4;

#endif // !VECTOR_H
