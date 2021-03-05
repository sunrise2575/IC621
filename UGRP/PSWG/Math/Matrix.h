#pragma once

#include <Prelude.h>

template<typename _Ty> class __matrix2 {
private:
	unsigned height;
	unsigned width;
	unsigned size;

	_Ty* pointer = nullptr;

public:
	__matrix2(unsigned height, unsigned width);
	__matrix2(const __matrix2& copyClass);
	~__matrix2();

	__matrix2& setZero();
	__matrix2& setRandom(_Ty minimum, _Ty maximum);
	__matrix2 tr(); // matrix transpose

	__matrix2& operator=(const __matrix2& copyClass);
	__matrix2 operator-();
	__matrix2 operator+(__matrix2& a);
	__matrix2 operator-(__matrix2& a);
	__matrix2 operator*(__matrix2& a);

	_Ty* operator[](unsigned idx);

	void print();
};

template<typename _Ty> __matrix2<_Ty>::__matrix2(unsigned height, unsigned width) {
	this->height = height;
	this->width = width;
	this->size = height*width;

	if (size == 0) throw Matrix2_SizeIsZero;

	this->pointer = new _Ty[size];

	this->setZero();
}

template<typename _Ty> __matrix2<_Ty>::__matrix2(const __matrix2& copyClass) {
	this->height = copyClass.height;
	this->width = copyClass.width;
	this->size = copyClass.height * copyClass.width;

	if (size == 0) throw Matrix2_SizeIsZero;

	this->pointer = new _Ty[copyClass.size];

	for (unsigned i = 0; i < copyClass.size; i++) {
		this->pointer[i] = copyClass.pointer[i];
	}
}

template<typename _Ty> __matrix2<_Ty>& __matrix2<_Ty>::operator=(const __matrix2<_Ty>& copyClass) {
	
	this->height = copyClass.height;
	this->width = copyClass.width;
	this->size = copyClass.height * copyClass.width;

	if (size == 0) throw Matrix2_SizeIsZero;

	delete[] this->pointer;
	this->pointer = new _Ty[copyClass.size];

	for (unsigned i = 0; i < copyClass.size; i++) {
		this->pointer[i] = copyClass.pointer[i];
	}
	return *this;
}

template<typename _Ty> __matrix2<_Ty>::~__matrix2() {
	if (pointer != nullptr) {
		delete[] pointer;
		pointer = nullptr;
	}
}

template<typename _Ty> __matrix2<_Ty>& __matrix2<_Ty>::setZero() {
	for (unsigned i = 0; i < size; i++)
		pointer[i] = _Ty(0);
	return *this;
}

template<typename _Ty> __matrix2<_Ty>& __matrix2<_Ty>::setRandom(_Ty minimum, _Ty maximum) {
	this->setZero();

	std::random_device rn;
	std::mt19937_64 rnd(rn());
	std::uniform_real_distribution<_Ty> range(minimum, maximum);
	for (unsigned i = 0; i < size; i++)
		pointer[i] = range(rnd);
	
	return *this;
}

template<typename _Ty> __matrix2<_Ty> __matrix2<_Ty>::tr() {
	__matrix2<_Ty> result(this->width, this->height);

	for (unsigned i = 0; i < this->width; i++) {
		for (unsigned j = 0; j < this->height; j++) {
			result[i][j] = (*this)[j][i];
		}
	}

	return result;
}

template<typename _Ty> void __matrix2<_Ty>::print() {
	for (unsigned i = 0; i < height; i++) {
		for (unsigned j = 0; j < width; j++) {
			printf("%  15.3f ", (*this)[i][j]);
		}
		printf("\n");
	}
}

template<typename _Ty> __matrix2<_Ty> __matrix2<_Ty>::operator-() {
	__matrix2<_Ty> result(this->height, this->width);
	for (unsigned i = 0; i < height; i++) {
		for (unsigned j = 0; j < width; j++) {
			result[i][j] = -(*this)[i][j];
		}
	}

	return result;
}

template<typename _Ty> __matrix2<_Ty> __matrix2<_Ty>::operator+(__matrix2& a) {
	unsigned result_height;
	unsigned result_width;

	(this->height >= a.height) ? result_height = this->height : result_height = a.height;
	(this->width >= a.width) ? result_width = this->width : result_width = a.width;

	__matrix2<_Ty> result(result_height, result_width);

	for (unsigned i = 0; i < result_height; i++) {
		for (unsigned j = 0; j < result_width; j++) {
			if ((i < this->height && j < this->width) && (i < a.height && j < a.width)) {
				result[i][j] = (*this)[i][j] + a[i][j];
			}
			else if ((i < this->height && j < this->width) && !(i < a.height && j < a.width)) {
				result[i][j] = (*this)[i][j];
			}
			else if (!(i < this->height && j < this->width) && (i < a.height && j < a.width)) {
				result[i][j] = a[i][j];
			}
			else {
				result[i][j] = 0;
			}
		}
	}
	return result;
}

template<typename _Ty> __matrix2<_Ty> __matrix2<_Ty>::operator-(__matrix2& a) {
	return (*this) + (-a);
}

template<typename _Ty> __matrix2<_Ty> __matrix2<_Ty>::operator*(__matrix2& a) {
	if (this->width != a.height) throw Matrix2_SizeMismatched;

	_Ty sum;
	__matrix2<_Ty> result(this->height, a.width);

	for (unsigned i = 0; i < this->height; i++) {
		for (unsigned j = 0; j < a.width; j++) {
			sum = 0;
			for (unsigned k = 0; k < this->width; k++) {
				sum += (*this)[i][k] * a[k][j];
			}
			result[i][j] = sum;
		}
	}
	return result;
}

template<typename _Ty> _Ty* __matrix2<_Ty>::operator[](unsigned idx) {
	return pointer + (idx * width);
}

typedef __matrix2<double> Matrix2;