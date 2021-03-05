#pragma once

#include <Prelude.h>

template<typename T> class __vector3 {
public:
	T x, y, z;

	__vector3() { this->__vector3<T>::__vector3(0); }
	__vector3(T a) { this->__vector3<T>::__vector3(a, a, a); }
	__vector3(T a, T b, T c) { x = a; y = b; z = c; }
	__vector3(const __vector3<T>& copy) { this->x = copy.x; this->y = copy.y; this->z = copy.z; }

	__vector3 operator+(const __vector3<T> &a) { return __vector3<T>(x + a.x, y + a.y, z + a.z); }
	__vector3 operator+(const T &a) { return *this + __vector3<T>(a); }
	__vector3& operator+=(const __vector3<T> &a) { *this = *this + a; return *this + a; }
	__vector3& operator+=(const T &a) { *this = *this + a; return *this + a; }

	__vector3 operator-() { return __vector3<T>(-x, -y, -z); }
	__vector3 operator-(const __vector3<T> &a) { return __vector3<T>(x - a.x, y - a.y, z - a.z); }
	__vector3 operator-(const T &a) { return *this + __vector3<T>(-a); }
	__vector3& operator-=(const __vector3<T> &a) { *this = *this - a; return *this; }
	__vector3& operator-=(const T &a) { *this = *this - a;  return *this; }

	__vector3 operator*(const T &a) { return __vector3<T>(x * a, y * a, z * a); }
	__vector3& operator*=(const T &a) { *this = *this * a; return *this; }

	__vector3 operator/(const T &a) { return __vector3<T>(x / a, y / a, z / a); }
	__vector3& operator/=(const T &a) { *this = *this / a; return *this; }

	bool operator==(const __vector3<T> &a) { return ((x == a.x) && (y == a.y) && (z == a.z)) ? true : false; }
	bool operator!=(const __vector3<T> &a) { return !(*this == a); }

	bool operator<(const __vector3<T> &a) { return ((x < a.x) && (y < a.y) && (z < a.z)) ? true : false; }
	bool operator<=(const __vector3<T> &a) { return (*this == a) || (*this < a); }
	bool operator>(const __vector3<T> &a) { return ((x > a.x) && (y > a.y) && (z > a.z)) ? true : false; }
	bool operator>=(const __vector3<T> &a) { return (*this == a) || (*this > a);; }
	//위와 같은 비교 연산에서는 (! a<b) != (a>=b) 이다!

	template<typename TyCast> operator __vector3<TyCast>() 
	{
		__vector3<TyCast> Result; 
		Result.x = (TyCast)x; Result.y = (TyCast)y; Result.z = (TyCast)z;
		return Result;
	}

	void print(){ std::cout << "| x =" << x << " y =" << y << " z =" << z << "|\n"; }
};

template<typename T> __vector3<T> v3rand(const __vector3<T> &max, const __vector3<T> &min)
{
	std::random_device rn;	std::mt19937_64 rnd(rn());
	std::uniform_real_distribution<T> rangeX(min.x, max.x);
	std::uniform_real_distribution<T> rangeY(min.y, max.y);
	std::uniform_real_distribution<T> rangeZ(min.z, max.z);
	return Vector3(rangeX(rnd), rangeY(rnd), rangeZ(rnd));
	//i think it is too slow;
};

template<typename T> T innerMulty(const __vector3<T> &a) { return a.x*a.y*a.z; }
template<typename T> T abs2(const __vector3<T> &a) { return a.x * a.x + a.y * a.y + a.z * a.z; }
template<typename T> T abs(const __vector3<T> &a) { return sqrt(abs2(a)); }
template<typename T> T dot(__vector3<T> &a, __vector3<T> &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
template<typename T> __vector3<T> cross(__vector3<T> &a, __vector3<T> &b) { return __vector3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
template<typename T> void displayV3(__vector3<T> &a) { std::cout << "(" << a.x << ", " << a.y << ", " << a.z << ")" << std::endl; }

typedef __vector3<phys_t> Vector3;

template<typename T> __vector3<T> operator+(T a, __vector3<T>& b) { return b + a; }
template<typename T> __vector3<T> operator-(T a, __vector3<T>& b) { return b - a; }
template<typename T> __vector3<T> operator*(T a, __vector3<T>& b) { return b * a; }

template<typename T> std::ostream& operator<<(std::ostream& out, const __vector3<T>& vector) {

	return out<<'(' << vector.x << ", " << vector.y << ", " << vector.z << ')';
}