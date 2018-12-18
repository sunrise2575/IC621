// Streams
#include <iostream>
#include <sstream>
#include <fstream>

// Containers
#include <string>
#include <vector>

// Time
#include <chrono>

// C headers
#include <cmath>
#include <cstdlib>
#include <cstring>

// CUDA headers
#include <cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>

int reverse_int32(const int i) {
	unsigned char byte1, byte2, byte3, byte4;
	byte1 = i & 255;
	byte2 = (i >> 8) & 255;
	byte3 = (i >> 16) & 255;
	byte4 = (i >> 24) & 255;
	return ( (int)byte1 << 24 ) + ( (int)byte2 << 16 ) + ( (int)byte3 << 8 ) + (int)byte4;
}

struct image_t {
	int magic_number;
	int size, row, column;
	float** data;
	image_t(const char* file_path, const int padding_width) {
		std::ifstream infile(file_path, std::ios::binary);
		if (!infile.is_open()) {
			printf("----ERROR: File open failure\n");
			exit(1);
		}

		infile.read((char*)&this->magic_number, sizeof(this->magic_number));
		this->magic_number = reverse_int32(this->magic_number);

		infile.read((char*)&this->size, sizeof(this->size));
		this->size = reverse_int32(this->size);

		infile.read((char*)&this->row, sizeof(this->row));
		this->row = reverse_int32(this->row);

		infile.read((char*)&this->column, sizeof(this->column));
		this->column = reverse_int32(this->column);

		int new_row = this->row + 2 * padding_width;
		int new_column = this->column + 2 * padding_width;

		this->data = (float**)malloc(sizeof(float*) * this->size);
		for(int i = 0; i < this->size; i++) {
			this->data[i] = (float*)malloc(sizeof(float) * new_row * new_column); // 패딩 할 걸 생각해서 잡는다
			memset(this->data[i], 0, sizeof(float) * new_row * new_column);
		}

		for(int image = 0; image < this->size; ++image) {
			for(int i = 0; i < this->row; ++i) {
				for(int j = 0; j < this->column; ++j) {
					unsigned char temp = 0;
					infile.read((char*)&temp, sizeof(temp));
					this->data[image][(i + padding_width) * new_row + (j + padding_width)] = (float)temp * (float)((float)1/(float)255);
				}
			}
		}

		this->row = new_row;
		this->column = new_column;

		infile.close();
	}

	~image_t() {
		for(int i = 0; i < size; i++) {
			free(this->data[i]);
		}
		free(this->data);
	}
};

struct label_t {
	int magic_number;
	int size;
	int* data;
	label_t(const char* file_path) {
		std::ifstream infile(file_path, std::ios::binary);
		if (!infile.is_open()) {
			printf("----ERROR: File open failure\n");
			exit(1);
		}

		infile.read((char*)&this->magic_number, sizeof(this->magic_number));
		this->magic_number = reverse_int32(this->magic_number);

		infile.read((char*)&this->size, sizeof(this->size));
		this->size = reverse_int32(this->size);

		this->data = (int*)malloc(sizeof(int) * this->size);

		for(int i = 0; i < this->size; ++i) {
			unsigned char temp = 0;
			infile.read((char*)&temp, sizeof(temp));
			this->data[i] = (int)temp;
		}

		infile.close();
	}
	~label_t() {
		if (this->data != nullptr) {
			free(this->data);
		}
	}
};

struct result_layers_t {
	float input[1 * 32 * 32];	// 입력 이미지 (제로패딩 된 것)
	float conv_1[6 * 28 * 28];	// 콘볼루션 결과 1
	float sub_1[6 * 14 * 14];	// 서브샘플링 결과 1
	float conv_2[16 * 10 * 10];	// 콘볼루션 결과 2
	float sub_2[16 * 5 * 5];	// 서브샘플링 결과 2
	float full_1[120];			// 뉴런 1
	float full_2[84];			// 뉴런 2
	float output[10];			// 출력
};

struct mask_bias_weight_t {
	float mask_1[6 * 1 * 5 * 5];	// 콘볼루션 마스크 1
	float bias_1[6];				// 바이어스 1
	float mask_2[16 * 6 * 5 * 5];	// 콘볼루션 마스크 2
	float bias_2[16];				// 바이어스 2
	float weight_1[120 * 400];		// 웨이트 1
	float bias_3[120];				// 바이어스 3
	float weight_2[84 * 120];		// 웨이트 2
	float bias_4[84];				// 바이어스 4
	float weight_3[10 * 84];		// 웨이트 3
	float bias_5[10];				// 바이어스 5
};

void print_mnist(const float* data, const int label) {
	std::cout << "Check data for label " << label << std::endl;
	for(int r = 0; r < 32; r++) {
		for (int c = 0; c < 32; c++) {
			if (data[r * 32 + c] > 0.5f) {
				std::cout << "■";
			} else {
				std::cout << "□";
			}
		}
		std::cout << std::endl;
	}
}

void parse_mask_bias_weight(const char* file_path, mask_bias_weight_t* filter) {
	std::ifstream infile(file_path);
	if (!infile.is_open()) {
		printf("----ERROR: File open failure\n");
		exit(1);
	}

	float* temp = (float*)malloc(sizeof(mask_bias_weight_t));

	std::string each_line;
	double num;
	int word_count = 0;
	while (std::getline(infile, each_line)) {
		if (each_line.length() == 0) { continue; }
		if (each_line[0] == '#') { continue; }

		std::stringstream ss;
		ss.str(each_line);
		while (ss >> num) {
			temp[word_count] = (float)num;
			word_count++;
		}
	}

	memcpy(filter, temp, sizeof(mask_bias_weight_t));

	free(temp);

	infile.close();
}

__device__ float sigmoid(float x) {
	return 1.0 / (1.0 + exp(-x));
}

__global__ void conv_forward(float* input, int input_count, int input_row, int input_column,
							 float* kernel, int kernel_count1, int kernel_count2, int kernel_row, int kernel_column,
							 float* output, int output_count, int output_row, int output_column)
{
	int m = blockIdx.x; // output image number
	int h = threadIdx.x; // output height
	int w = threadIdx.y; // output width

	float accumulated = 0;
	for (int c = 0; c < input_count; c++) {
		for (int p = 0; p < kernel_row; p++) {
			for (int q = 0; q < kernel_column; q++) {
				accumulated += input[input_row * input_column * c + input_column * (h + p) + (w + q)]
				* kernel[kernel_count2 * kernel_row * kernel_column * m + kernel_row * kernel_column * c + kernel_column * p + q];
			}
		}
	}
	output[output_row * output_column * m + output_column * h + w] = accumulated;

}

__global__ void pool_forward(float* input, int input_count, int input_row, int input_column,
							 float* bias,
							 float* output, int output_count, int output_row, int output_column)
{
	int m = blockIdx.x; // output image number
	int h = threadIdx.x; // output height
	int w = threadIdx.y; // output width

	float accumulated = 0;
	for (int p = 0; p < 2; p++) {
		for (int q = 0; q < 2; q++) {
			accumulated += input[input_row * input_column * m + input_column * (2 * h + p) + (2 * w + q)] / (2 * 2);
		}
	}
	output[output_row * output_column * m + output_column * h + w]
	= sigmoid(accumulated + bias[m]);
}

__device__ void full_forward(float* input,
							 float* weight, int weight_row, int weight_column,
							 float* bias,
							 float* output)
{
	int i = threadIdx.x; // weight row
	if (i < weight_row) {
		for (int k = 0; k < weight_column; k++) {
			output[i] += weight[weight_column * i + k] * input[k];
		}
		output[i] = sigmoid(output[i] + bias[i]);
	}
}

__global__ void full_forward_total(result_layers_t* d_result, mask_bias_weight_t* d_filter) {
	int image = blockIdx.x;

	// 서브샘플링과 웨이트 및 바이어스로 다음 뉴런 연산
	full_forward(d_result[image].sub_2,
				 d_filter->weight_1, 120, 400,
				 d_filter->bias_3,
				 d_result[image].full_1);

	__syncthreads();

	// 뉴런 결과와 웨이트 및 바이어스로 다음 뉴런 연산
	full_forward(d_result[image].full_1,
				 d_filter->weight_2, 84, 120,
				 d_filter->bias_4,
				 d_result[image].full_2);

	__syncthreads();


	// 뉴런 결과와 웨이트 및 바이어스로 출력 연산
	full_forward(d_result[image].full_2,
				 d_filter->weight_3, 10, 84,
				 d_filter->bias_5,
				 d_result[image].output);

	__syncthreads();
}

int find_max_index(float* input, const int size) {
	int result = 0;
	float max_value = -9.99;
	for (int i = 0; i < size; i++) {
		if (input[i] > max_value) {
			max_value = input[i];
			result = i;
		}
	}

	return result;
}

int main() {
	// MNIST 파일 불러오기
    image_t train_image("/home/ic621/mnist/train-images-idx3-ubyte", 2);
    label_t train_label("/home/ic621/mnist/train-labels-idx1-ubyte");
    image_t test_image("/home/ic621/mnist/t10k-images-idx3-ubyte", 2);
	label_t test_label("/home/ic621/mnist/t10k-labels-idx1-ubyte");

    // result_layers에 적재하기
    result_layers_t* train_result = (result_layers_t*)malloc(sizeof(result_layers_t) * train_image.size);
    result_layers_t* test_result = (result_layers_t*)malloc(sizeof(result_layers_t) * test_image.size);

    for (int image = 0; image < train_image.size; image++) {
        for (int i = 0; i < train_image.row; i++) {
            for (int j = 0; j < train_image.column; j++) {
                train_result[image].input[32 * i + j] = train_image.data[image][i * train_image.row + j];
            }
        }
    }

    for (int image = 0; image < test_image.size; image++) {
        for (int i = 0; i < test_image.row; i++) {
            for (int j = 0; j < test_image.column; j++) {
                test_result[image].input[32 * i + j] = test_image.data[image][i * test_image.row + j];
            }
        }
    }

	// 필터 불러오기
	mask_bias_weight_t filter;
	parse_mask_bias_weight("text_out_lenet5.txt", &filter);

	int total = test_image.size;

	// test_result와 filter모두 cuda로 보내기
	result_layers_t* d_result;
	mask_bias_weight_t* d_filter;

	cudaMalloc(&d_result, sizeof(result_layers_t) * total);
	cudaMalloc(&d_filter, sizeof(mask_bias_weight_t));

	cudaMemcpy(d_result, test_result, sizeof(result_layers_t) * total, cudaMemcpyHostToDevice);
	cudaMemcpy(d_filter, &filter, sizeof(mask_bias_weight_t), cudaMemcpyHostToDevice);

	auto start = std::chrono::high_resolution_clock::now();

	for (int image = 0; image < total; image++) {
		// 입력이미지와 콘볼루션 마스크로 콘볼루션 연산 수행
		{
			dim3 blocks(6, 1, 1); // output map 개수를 써준다
			dim3 threads(28, 28, 1); // output row/column 개수를 써준다
			conv_forward<<<blocks, threads>>>(d_result[image].input, 1, 32, 32,
											  d_filter->mask_1, 6, 1, 5, 5,
											  d_result[image].conv_1, 6, 28, 28);
		}

		// 콘볼루션 결과와 바이어스로 서브샘플링 수행
		{
			dim3 blocks(6, 1, 1); // output map 개수를 써준다
			dim3 threads(14, 14, 1); // output row/column 개수를 써준다
			pool_forward<<<blocks, threads>>>(d_result[image].conv_1, 6, 28, 28,
				d_filter->bias_1,
				d_result[image].sub_1, 6, 14, 14);
		}

		// 서브샘플링 결과와 콘볼루션 마스크로 콘볼루션 연산 수행
		{
			dim3 blocks(16, 1, 1); // output map 개수를 써준다
			dim3 threads(10, 10, 1); // output row/column 개수를 써준다
			conv_forward<<<blocks, threads>>>(d_result[image].sub_1, 6, 14, 14,
											  d_filter->mask_2, 16, 6, 5, 5,
											  d_result[image].conv_2, 16, 10, 10);
		}

		// 콘볼루션 결과와 바이어스로 서브샘플링 수행
		{
			dim3 blocks(16, 1, 1); // output map 개수를 써준다
			dim3 threads(5, 5, 1); // output row/column 개수를 써준다
			pool_forward<<<blocks, threads>>>(d_result[image].conv_2, 16, 10, 10,
				d_filter->bias_2,
				d_result[image].sub_2, 16, 5, 5);
		}
	}

	// fully connected는 갈수록 행렬의 크기가 감소하며, 계산량이 많지 않기에,
	// image별로 block을 생성하여 한꺼번에 계산하였다.
	// block개수는 65536이 최대. test가 10000개 이미지이므로 괜찮다.
	{
		dim3 blocks(total, 1, 1);
		dim3 threads(120, 1, 1);
		full_forward_total<<<blocks, threads>>>(d_result, d_filter);
	}

	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);

	// GPU에서 계산한 거를 옮긴다
	cudaMemcpy(test_result, d_result, sizeof(result_layers_t) * total, cudaMemcpyDeviceToHost);

	// 정확도를 구한다
	int accurate = 0;
	for (int image = 0; image < total; image++) {
		int result = find_max_index(test_result[image].output, 10);
		int answer = test_label.data[image];
		if (result == answer) {
			accurate++;
		}
	}

	printf("test data accuracy: %8lf\n", (float)accurate / (float)total);
	printf("elapsed time      : %8lf (sec)\n", elapsed.count());

    free(train_result);
    free(test_result);

    return 0;
}
