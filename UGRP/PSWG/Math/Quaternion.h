#pragma once

template<typename _Ty> class __quaternion {
public:
	_Ty a, x, y, z;

	__quaternion() { this->__quaternion::__quaternion<_Ty>(_Ty(0), _Ty(0), _Ty(0), _Ty(0)); }
	__quaternion(const __quaternion& copy) { this->a = copy.a; this->x = copy.x; this->y = copy.y; this->z = copy.z; }
	__quaternion(_Ty a, _Ty x, _Ty y, _Ty z) { this->a = a; this->x = x; this->y = y;	this->z = z; }

	__quaternion operator-() { return __quaternion<_Ty>(-a, -x, -y, -z); }

	__quaternion operator+(const __quaternion& q) {	return __quaternion<_Ty>(a + q.a, x + q.x, y + q.y, z + q.z); }
	__quaternion operator-(const __quaternion& q) { return __quaternion<_Ty>(a - q.a, x - q.x, y - q.y, z - q.z); }
	__quaternion operator*(const __quaternion& q) {	return __quaternion<_Ty>(a * q.a - x * q.x - y * q.y - z * q.z,
																	   a * q.x + x * q.a + y * q.z - z * q.y,
																	   a * q.y - x * q.z + y * q.a + z * q.x,
																	   a * q.z + x * q.y - y * q.x + z * q.a); }
	unsigned size() {	return (a * a + x * x + y * y + z * z); }
	__quaternion conj() { return __quaternion<_Ty>(a, -x, -y, -z); } // conjugate
};

typedef __quaternion<float> Quaternion;