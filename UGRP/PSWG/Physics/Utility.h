#pragma once

#include <Prelude.h>
#include _CH_VECTOR_
#include _CH_MATRIX_

#define PI 3.141592

namespace Util {
	phys_t v3Distance(Vector3 position1, Vector3 position2) {
		return abs(position1 - position2);
	}

	Vector3 v3Direction(Vector3 from, Vector3 to) {
		return (to - from) / abs(to - from);
	}

	unsigned Index2(int index1, int index2, int length_index1) {
		return index1 + length_index1 * index2;
	}

	unsigned Index3(int index1, int index2, int index3, int length_index1, int length_index2) {
		return Index2(index1, Index2(index2, index3, length_index2), length_index1);
	}

	unsigned Index3(__vector3<unsigned> v3Index, int length_index1, int length_index2)
	{
		return Index3(v3Index.x, v3Index.y, v3Index.z, length_index1, length_index2);
	}

	__vector3<unsigned> v2Index(int index, int length_index1)
	{
		return __vector3<unsigned>(index%length_index1, index / length_index1, 0);
	}

	__vector3<unsigned> v3Index(int index, int length_index1, int length_index2)
	{
		//return __vector3<unsigned>(div(div(index, length_index1*length_index2).rem, length_index1).rem, div(div(index, length_index1*length_index2).rem, length_index1).quot, div(index, length_index1*length_index2).quot);
		return __vector3<unsigned>(index%length_index1, (index / length_index1) % length_index2, (index / (length_index1*length_index2)));
	}

	template <typename T> inline void clamp(T& variable, T max, T min)
	{
		if (variable > max) { variable = max; }
		else if (variable < min) { variable = min; }
	}

	template <typename T, unsigned SIZE> inline void resizeDynamicArray(T(& arrPtr)[SIZE], int size)
	{
		T* temp = new T[size];
		memcpy(temp, &arrPtr, sizeof(T)*SIZE);
		&arrPtr = temp;
	}

	template <typename T> __matrix2<T> rotationMatrixX(T Rotation)
	{
		__matrix2<T> tempRotation(3, 3);
		tempRotation.setZero();
		tempRotation[1][1] = cos(Rotation);
		tempRotation[1][2] = -sin(Rotation);
		tempRotation[2][1] = sin(Rotation);
		tempRotation[2][2] = cos(Rotation);
		tempRotation[0][0] = 1;
		return tempRotation;
	}
	template <typename T> __matrix2<T> rotationMatrixY(T Rotation)
	{
		__matrix2<T> tempRotation(3, 3);
		tempRotation.setZero();
		tempRotation[0][0] = cos(Rotation);
		tempRotation[2][0] = -sin(Rotation);
		tempRotation[0][2] = sin(Rotation);
		tempRotation[2][2] = cos(Rotation);
		tempRotation[1][1] = 1;
		return tempRotation;
	}
	template <typename T> __matrix2<T> rotationMatrixZ(T Rotation)
	{
		__matrix2<T> tempRotation(3, 3);
		tempRotation.setZero();
		tempRotation[0][0] = cos(Rotation);
		tempRotation[0][1] = -sin(Rotation);
		tempRotation[1][0] = sin(Rotation);
		tempRotation[1][1] = cos(Rotation);
		tempRotation[2][2] = 1;
		return tempRotation;
	}
	template <typename T> __matrix2<T> rotationMatrix(__vector3 <T> Rotation)
	{
		__matrix2<T> tempRotation(3, 3);
		tempRotation = rotationMatrixY(Rotation.y)*rotationMatrixX(Rotation.x)*rotationMatrixZ(Rotation.z);
		return tempRotation;
	}
	template <typename T> __matrix2<T> rotationMatrixReverse(__vector3 <T> Rotation)
	{
		__matrix2<T> tempRotation(3, 3);
		tempRotation = rotationMatrixZ(-Rotation.z)*rotationMatrixX(-Rotation.x)*rotationMatrixY(-Rotation.y);
		return tempRotation;
	}
}