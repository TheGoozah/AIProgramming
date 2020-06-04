/*=============================================================================*/
// Copyright 2017-2018 Elite Engine
// Authors: Matthieu Delaere, Thomas Goussaert
/*=============================================================================*/
// EMathUtilities.h: Utility class containing a bunch of commonely used functionality (not custom-type specific)
/*=============================================================================*/
#ifndef ELITE_MATH_UTILITIES
#define ELITE_MATH_UTILITIES
//Standard C++ includes
#include <cstdlib>
#include <cfloat>
#include <type_traits>

namespace Elite {
	/* --- CONSTANTS --- */
	#define E_PI	3.14159265358979323846
	#define E_PI_2	1.57079632679489661923
	#define E_PI_4	0.785398163397448309616

	/* --- FUNCTIONS --- */
	/*! Comparing two values (preferably float or doubles) and see if they are equal. You can change the precision (by default: epsilon)*/
	template<typename T, typename = std::enable_if<std::is_pod<T>::value>>
	constexpr bool AreEqual(T a, T b, float precision = FLT_EPSILON)
	{
		if (abs(a - b) > precision)
			return false;
		return true;
	}
	/*! An accurate inverse square root*/
	inline float InvSqrt(float f) //sqrtf not defined as constexpr
	{
		return 1.0f / sqrtf(f);
	}
	/*! An fast inverse square root, not fully accurate. Implementation based on Quake III Arena*/
	/*! Reference: https://betterexplained.com/articles/understanding-quakes-fast-inverse-square-root/ */
	constexpr float InvSqrtFst(float f)
	{
		const float xHalf = 0.5f * f;
		int i = *reinterpret_cast<int*>(&f);
		i = 0x5f3759df - (i >> 1);
		f = *reinterpret_cast<float*>(&i);
		f = f*(1.5f - xHalf*f*f);
		return f;
	}
	/*! Function to square a number */
	template<typename T, typename = std::enable_if<std::is_pod<T>::value>>
	constexpr auto Square(T v)
	{
		return v*v;
	}
	/*! Function to convert degrees to radians */
	constexpr float ToRadians(const float angle)
	{
		return angle * (static_cast<float>(E_PI) / 180.f);
	}
	/*! Template function to clamp between a minimum and a maximum value -> in STD since c++17 */
	template<typename T>
	constexpr T Clamp(const T a, T min, T max)
	{
		if (a < min)
			return min;

		if (a > max)
			return max;

		return a;
	}
	/*! Random Integer */
	inline int randomInt(int max = 1)
	{ return rand() % max; }

	/*! Random Float */
	inline float randomFloat(float max = 1.f)
	{ return max * (float(rand()) / RAND_MAX); }

	/*! Random Binomial Float */
	inline float randomBinomial(float max = 1.f)
	{ return randomFloat(max) - randomFloat(max); }
}
#endif