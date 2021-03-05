// SSE and AVX
#include <immintrin.h>

// C++
#include <iostream>
#include <sys/time.h>

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
	float *a, *b;
	float *pA, *pB;
	__m128 rA, rB;
	__m256 rA_AVX, rB_AVX;

	const int vector_size = 10000000;
	
	a = (float*)_mm_malloc(vector_size * sizeof(float), 32);
	b = (float*)_mm_malloc(vector_size * sizeof(float), 32);

	for (int i = 0; i < vector_size; i++) {
		a[i] = fabs(drand48());
		b[i] = 0.0f;
	}

	// Naive

	s = getCurrentTime();
	for (int i = 0; i < vector_size; i++) {
		b[i] = sqrtf(sqrtf(sqrtf(a[i])));
	}

	e = getCurrentTime();
	std::cout << (e - s) * 1000 << " msec" << ", b[42] = " << b[42] << std::endl;

	for (int i = 0; i < vector_size; i++) {
		b[i] = 0.0f;
	}

	// SSE2
	pA = a;
	pB = b;

	s = getCurrentTime();
	for (int i = 0; i < vector_size; i+=4) {
		rA = _mm_load_ps(pA);
		rB = _mm_sqrt_ps(_mm_sqrt_ps(_mm_sqrt_ps(rA)));
		_mm_store_ps(pB, rB);
		pA += 4;
		pB += 4;		
	}

	e = getCurrentTime();
	std::cout << (e - s) * 1000 << " msec" << ", b[42] = " << b[42] << std::endl;

	for (int i = 0; i < vector_size; i++) {
		b[i] = 0.0f;
	}
	
	// AVX
	pA = a;
	pB = b;

	s = getCurrentTime();
	for (int i = 0; i < vector_size; i+=8) {
		rA_AVX = _mm256_load_ps(pA);
		rB_AVX = _mm256_sqrt_ps(_mm256_sqrt_ps(_mm256_sqrt_ps(rA_AVX)));
		_mm256_store_ps(pB, rB_AVX);
		pA += 8;
		pB += 8;
	}

	e = getCurrentTime();
	std::cout << (e - s) * 1000 << " msec" << ", b[42] = " << b[42] << std::endl;

	_mm_free(a);
	_mm_free(b);	

	return 0;
}
