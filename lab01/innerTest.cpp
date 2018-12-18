// SSE and AVX
#include <immintrin.h>

// C++
#include <iostream>
#include <sys/time.h>
#include <iostream>

// C
#include <cmath>

double getCurrentTime() {
	struct timeval curr;
	struct timezone tz;
	gettimeofday(&curr, &tz);
	double tmp = static_cast<double>(curr.tv_sec) * static_cast<double>(1000000)
			   + static_cast<double>(curr.tv_usec);
	return tmp * 1e-6;
}

int main() {
	srand48(0);
	double e, s;
	float *a, *b, *c;
	float *pA, *pB, *pC;
	float *r, *r2;
	float *pR;
	float result;

	const int iteration = 1000;
	const int vector_size = 4;
	
	a = (float*)_mm_malloc(vector_size * sizeof(float), 32);
	b = (float*)_mm_malloc(vector_size * sizeof(float), 32);
	c = (float*)_mm_malloc(vector_size * sizeof(float), 32);
	r = (float*)_mm_malloc(4 * sizeof(float), 32);
	r2 = (float*)_mm_malloc(8 * sizeof(float), 32);

	for (int i = 0; i < vector_size; i++) {
		a[i] = fabs(drand48());
		b[i] = fabs(drand48());
		c[i] = 0.0f;
	}

	for (int i = 0; i < 4; i++) {
		r[i] = 0.0f;
	}

	// Naive

	result = 0.0f;

	s = getCurrentTime();
	for (int j = 0; j < iteration; j++) {
		for (int i = 0; i < vector_size; i++) {
			c[i] = a[i] * b[i];
		}
	}

	for (int i = 0; i < vector_size; i++) {
		result += c[i];			
	}
	e = getCurrentTime();

	std::cout << (e - s) * 1000 << " msec" << ", result = " << result << std::endl;
	for (int i = 0; i < vector_size; i++) {
		c[i] = 0.0f;
	}

	// SSE2
	__m128 rA, rB, rC, rR;

	s = getCurrentTime();
	for (int j = 0; j < iteration; j++) {
		result = 0.0f; pA = a; pB = b; pC = c;
		for (int i = 0; i < vector_size; i+=4) {
			rA = _mm_load_ps(pA);
			rB = _mm_load_ps(pB);
			rC = _mm_mul_ps(rA, rB);
			_mm_store_ps(pC, rC);
			pA += 4;
			pB += 4;
			pC += 4;		
		}
	}

	pC = c; pR = r;
	for (int i = 0; i < vector_size; i+=4) {
		rR = _mm_load_ps(pR);
		rR = _mm_add_ps(rR, rC);
		_mm_store_ps(pR, rR);
		pC += 4;
	}

	for (int i = 0; i < 4; i++) {
		result += r[i];
	}
	e = getCurrentTime();
	std::cout << (e - s) * 1000 << " msec" << ", result = " << result << std::endl;

	for (int i = 0; i < vector_size; i++) {
		c[i] = 0.0f;
	}

	// AVX
	__m256 rA_AVX, rB_AVX, rC_AVX, rR_AVX;
	
	s = getCurrentTime();
	for (int j = 0; j < iteration; j++) {
		result = 0.0f; pA = a; pB = b; pC = c;
		for (int i = 0; i < vector_size; i+=8) {
			rA_AVX = _mm256_load_ps(pA);
			rB_AVX = _mm256_load_ps(pB);
			rC_AVX = _mm256_mul_ps(rA_AVX, rB_AVX);
			_mm256_store_ps(pC, rC_AVX);
			pA += 8;
			pB += 8;
			pC += 8;
		}

	}

	pC = c; pR = r2;
	for (int i = 0; i < vector_size; i+=8) {
		rR_AVX = _mm256_load_ps(pR);
		rR_AVX = _mm256_add_ps(rR_AVX, rC_AVX);
		_mm256_store_ps(pR, rR_AVX);
		pC += 4;
	}

	for (int i = 0; i < 8; i++) {
		result += r2[i];
	}
	e = getCurrentTime();
	std::cout << (e - s) * 1000 << " msec" << ", result = " << result << std::endl;

	_mm_free(a);
	_mm_free(b);
	_mm_free(c);
	_mm_free(r);
	_mm_free(r2);

	return 0;
}
