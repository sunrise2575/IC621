#include <cuda_runtime.h>
#include <cuda.h>
#include <device_launch_parameters.h>

#include <curand.h>
#include <curand_kernel.h>

#include <iostream>
#include <chrono>

#include <cstdlib>
#include <cmath>

// the max number of (x,y) threads is 1024
// which is 1024 = 32 x 32, so 0 <= threadIdx.x < 32 and 0 <= threadIdx.y < 32
constexpr unsigned int THREAD_COUNT = 32;

#define TILE_WIDTH 32

__global__ void kernel_fill_random(float* matrix, const unsigned int width) {
	// index
	const unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;
	const unsigned int j = blockIdx.y * blockDim.y + threadIdx.y;

	// set random seed
	unsigned int seed = i + j;
	curandState s;

	curand_init(seed, 0, 0, &s);

	//fill matrix
	if (i < width && j < width) {
		matrix[i * width + j] = curand_uniform(&s);
	}
}

__global__ void kernel_naive(const float* M, const float* N, float* P, const unsigned int width) {
	// index
	const unsigned int i = blockIdx.y * blockDim.y + threadIdx.y;
	const unsigned int j = blockIdx.x * blockDim.x + threadIdx.x;

	if (i < width && j < width) {
		float result = 0;

		for (int k = 0; k < width; ++k) {
			result += M[i * width + k] * N[k * width + j];
		}

		P[i * width + j] = result;
	}

}

__global__ void kernel_tiled(const float* M, const float* N, float* P, const int width) {
	__shared__ float Ms[TILE_WIDTH][TILE_WIDTH];
    __shared__ float Ns[TILE_WIDTH][TILE_WIDTH];

	int bx = blockIdx.x; int by = blockIdx.y;
	int tx = threadIdx.x; int ty = threadIdx.y;

	int row = by * TILE_WIDTH + ty;
	int col = bx * TILE_WIDTH + tx;

	float result = 0;
	const unsigned int TILE_COUNT = (unsigned int)std::ceil((float)width/(float)TILE_WIDTH);
	for (int m = 0; m < TILE_COUNT; ++m) {
		if (m * TILE_WIDTH + tx < width && row < width) {
			Ms[ty][tx] = M[row * width + (m * TILE_WIDTH + tx)];
		} else {
			Ms[ty][tx] = 0.0;
		}

		if (m * TILE_WIDTH + ty < width && col < width) {
			Ns[ty][tx] = N[col + (m * TILE_WIDTH + ty) * width];
		} else {
			Ns[ty][tx] = 0.0;
		}
		__syncthreads();

		for (int k = 0; k < TILE_WIDTH; ++k) {
			result += Ms[ty][k] * Ns[k][tx];
		}
		__syncthreads();
	}

	if (row < width && col < width) {
		P[row * width + col] = result;
	}
}

double naive(const float* d_M, const float* d_N, float* d_P, const unsigned int width) {
	const unsigned int block_count = (unsigned int)std::ceil((float)width/(float)THREAD_COUNT);
	dim3 blocks(block_count, block_count);
	dim3 threads(THREAD_COUNT, THREAD_COUNT);

	auto start = std::chrono::high_resolution_clock::now();

	kernel_naive<<<blocks, threads>>>(d_M, d_N, d_P, width);
	cudaDeviceSynchronize();

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);

	return elapsed.count();
}

double tiled(const float* d_M, const float* d_N, float* d_P, const unsigned int width) {
	const unsigned int block_count = (unsigned int)std::ceil((float)width/(float)TILE_WIDTH);
	dim3 blocks(block_count, block_count);
	dim3 threads(TILE_WIDTH, TILE_WIDTH);

	auto start = std::chrono::high_resolution_clock::now();

	kernel_tiled<<<blocks, threads>>>(d_M, d_N, d_P, width);
	cudaDeviceSynchronize();

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);

	return elapsed.count();
}

void file_load(const char* filepath, float* matrix, const size_t size) {
	FILE* file;
	size_t result;

	file = fopen(filepath, "rb");
	if (!file) {
		printf(">>>ERROR: opening %s\n", filepath);
		exit(1);
	}

	fseek(file, 0, SEEK_END);
	result = ftell(file);
	rewind(file);

	if (size != result) {
		printf(">>>ERROR: wrong filesize %ld\n", size);
		exit(1);
	}

	result = fread(matrix, 1, size, file);
	if (result != size) {
		printf(">>>ERROR: reading %s\n", filepath);
		exit(1);
	}

	fclose(file);
}

void file_save(float* matrix, const char* filepath, const size_t size) {
	FILE* file;
	size_t result;

	file = fopen(filepath, "wb");
	if (!file) {
		printf(">>>ERROR: opening %s\n", filepath);
		exit(1);
	}

	result = fwrite(matrix, 1, size, file);
	if (result != size) {
		printf(">>>ERROR: writing %s\n", filepath);
		exit(1);
	}

	fclose(file);
}

bool is_same(const float* matrix1, const float* matrix2, const size_t width) {
	for (size_t i = 0; i < width * width; i++) {
		if (matrix1[i] != matrix2[i]) {
			printf("position: %ld\n", i);
			return false;
		}
	}

	return true;
}

int main(int argc, char** argv) {
	if (argc != 5) { return 0; }

	const char* filepath_M = argv[1];
	const char* filepath_N = argv[2];
	const char* filepath_P = argv[3];
	const unsigned int width = atoi(argv[4]);

	const size_t size = width * width * sizeof(float);

	float* M = (float*)malloc(size);
	float* N = (float*)malloc(size);
	float* P1 = (float*)malloc(size);
	float* P2 = (float*)malloc(size);
	printf(">  COMPLETE: malloc on host\n");

	file_load(filepath_M, M, size);
	printf(">  COMPLETE: load %s\n", filepath_M);

	file_load(filepath_N, N, size);
	printf(">  COMPLETE: load %s\n", filepath_N);

	float *d_M, *d_N, *d_P;
	cudaMalloc(&d_M, size); cudaMalloc(&d_N, size); cudaMalloc(&d_P, size);
	cudaDeviceSynchronize();
	printf(">  COMPLETE: malloc on device\n");

	cudaMemcpy(d_M, M, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_N, N, size, cudaMemcpyHostToDevice);
	printf(">  COMPLETE: memcpy to device\n");

	double elapsed_naive = naive(d_M, d_N, d_P, width);
	printf(">>>RESULT: naive elapsed time: %8lf (sec)\n", elapsed_naive);
	cudaMemcpy(P1, d_P, size, cudaMemcpyDeviceToHost);
	cudaDeviceSynchronize();

	double elapsed_tiled = tiled(d_M, d_N, d_P, width);
	printf(">>>RESULT: elapsed time: %8lf (sec)\n", elapsed_tiled);
	cudaMemcpy(P2, d_P, size, cudaMemcpyDeviceToHost);

	if (is_same(P1, P2, width)) {
		printf(">  COMPLETE: no difference between results of naive and tiled method\n");
	} else {
		printf(">>>ERROR: difference between results of naive and tiled method\n");
		exit(1);
	}

	file_save(P2, filepath_P, size);
	printf(">  COMPLETE: save result matrix on %s\n", filepath_P);

	// free memory on device
	cudaFree(d_M); cudaFree(d_N); cudaFree(d_P);

	// free memory on host
	free(M); free(N); free(P1); free(P2);
}
