// C++11 headers
#include <random>
#include <iostream>
#include <chrono>

// for std::memcpy
#include <cstring>

// mpi
#include <mpi.h>

class custom_array {
private:
	// MPI 현재 상태
	MPI_Status status;

	// 배열 포인터
	int* arr = nullptr;

	// 배열 크기
	int arr_size;

	// MPI 정보들
	int rank;
	int process_count;

	bool is_sorted(int* __array, int __length) const {
		for (int i = 0; i < __length; i++) {
			// if i reaches to the end of the array
			if (i == __length - 1) {
				break;
			}
			// if former array element is larger than later array element
			if (__array[i] > __array[i+1]) {
				return false;
			}
		}
		return true;
	}

	void radix_sort(int *__array, int __length) {
		int i;

		// 최대값 찾기
		int max_number = __array[0];
		for (i = 1; i < __length; i++) {
			if (__array[i] > max_number) {
				max_number = __array[i];
			}
		}

		// 동적 할당
		int* buffer = (int*)malloc(sizeof(int) * __length);

		int exponent = 1;
		// 각 자리수에 대해 루프를 실행한다.
		while (max_number / exponent > 0) {
			int bucket_decimal[10] = {0};
			// 각 자리수에서 0~9까지의 숫자가 몇개 나오는지 센다.
			for (i = 0; i < __length; i++) {
				bucket_decimal[(__array[i] / exponent) % 10]++;
			}

			// 카운터 준비
			for (i = 1; i < 10; i++) {
				bucket_decimal[i] += bucket_decimal[i - 1];
			}

			// 숫자 정렬
			for (i = __length - 1; i >= 0; i--) {
				buffer[--bucket_decimal[(__array[i] / exponent) % 10]] = __array[i];
			}

			// 정렬 결과 복사
			for (i = 0; i < __length; i++) {
				__array[i] = buffer[i];
			}

			// 다음 자리수로 넘어간다
			exponent *= 10;
		}

		// 메모리 해제
		if (buffer != nullptr) {
			free(buffer);
		}
	}

	void extract_small_group(const int* __array_1, const int* __array_2, const int __array_size, int* __result) {
		int i = 0, j = 0, k = 0;
		while ((k < __array_size) && (i < __array_size) && (j < __array_size)) {
			if (__array_1[i] < __array_2[j]) {
				__result[k] = __array_1[i];
				i++;
			} else {
				__result[k] = __array_2[j];
				j++;
			}
			k++;
		}
	}

	void extract_large_group(const int* __array_1, const int* __array_2, const int __array_size, int* __result) {
		int i = __array_size - 1, j = __array_size - 1, k = __array_size - 1;
		while ((k >= 0) && (i >= 0) && (j >= 0)) {
			if (__array_1[i] > __array_2[j]) {
				__result[k] = __array_1[i];
				i--;
			} else {
				__result[k] = __array_2[j];
				j--;
			}
			k--;
		}
	}

public:
	custom_array(int __size, int __argc, char **__argv) {
		this->arr_size = __size;

		int result = MPI_Init(&__argc, &__argv);
		if (result != MPI_SUCCESS) {
			std::cerr << "MPI Error!" << std::endl;
			MPI_Abort(MPI_COMM_WORLD, result);
		}

		MPI_Comm_size(MPI_COMM_WORLD, &this->process_count);
		MPI_Comm_rank(MPI_COMM_WORLD, &this->rank);

		if (this->rank == 0) {
			this->arr = (int*)malloc(this->arr_size * sizeof(int));
		}
	}

	~custom_array() noexcept {
		if (this->rank == 0) {
			if (this->arr != nullptr) {
				free(this->arr);
			}
		}

		MPI_Finalize();
	}

	int size() const {
		return this->arr_size;
	}

	int get_rank() const {
		return this->rank;
	}

	int get_process_count() const {
		return this->process_count;
	}

	bool is_sorted() const {
		for (int i = 0; i < this->arr_size; i++) {
			// if i reaches to the end of the array
			if (i == this->arr_size - 1) {
				break;
			}
			// if former array element is larger than later array element
			if (this->arr[i] > this->arr[i+1]) {
				return false;
			}
		}
		return true;
	}

	void print(int* __array, int __length) const {
		for (int i = 0; i < __length; i++) {
			printf("%d ", __array[i]);
		}
		printf("\n");
	}

	void fill_randomly(const int __min, const int __max) {
		if (this->rank == 0) {
			for (int i = 0; i < this->arr_size; i++) {
				srand((unsigned int)time(NULL));
				this->arr[i] = rand() % (__max - __min + 1) + __min;
			}

		}
	}

	void odd_even_sort_mpi() {
		int segment_size = this->arr_size / this->process_count;

		// 나눠줄 조각을 받을 메모리를 할당한다.
		int* my_segment = (int*)malloc(sizeof(int) * segment_size);
		int* opponent_segment = (int*)malloc(sizeof(int) * segment_size);
		int* temp_segment = (int*)malloc(sizeof(int) * segment_size);

		// 깨끗하게 0으로 청소한다.
		memset(my_segment, 0x00, sizeof(int) * segment_size);
		memset(opponent_segment, 0x00, sizeof(int) * segment_size);
		memset(temp_segment, 0x00, sizeof(int) * segment_size);

		MPI_Barrier(MPI_COMM_WORLD);

		// 먼저 데이터를 흩뿌린다.
		MPI_Scatter(this->arr, segment_size, MPI_INT,
			my_segment, segment_size, MPI_INT,
			0, MPI_COMM_WORLD);

		// 각자 받은 것을 radix sort한다.
		this->radix_sort(my_segment, segment_size);

		MPI_Barrier(MPI_COMM_WORLD);

		// 이제 페이즈를 바꾸면서 한다
		for (int phase = 0; this->process_count != 1 && phase < this->process_count; phase++) {
			if (phase % 2 == 0) {
				// 이 페이즈는 0-1 2-3 4-5 6-7 이다.
				if (this->rank % 2 == 0) {
					MPI_Send(my_segment, segment_size, MPI_INT, this->rank + 1, 0, MPI_COMM_WORLD);
					MPI_Recv(opponent_segment, segment_size, MPI_INT, this->rank + 1, 0, MPI_COMM_WORLD, &this->status);
					this->extract_small_group(my_segment, opponent_segment, segment_size, temp_segment);
					memcpy(my_segment, temp_segment, sizeof(int) * segment_size);
				} else {
					MPI_Recv(opponent_segment, segment_size, MPI_INT, this->rank - 1, 0, MPI_COMM_WORLD, &this->status);
					MPI_Send(my_segment, segment_size, MPI_INT, this->rank - 1, 0, MPI_COMM_WORLD);
					this->extract_large_group(my_segment, opponent_segment, segment_size, temp_segment);
					memcpy(my_segment, temp_segment, sizeof(int) * segment_size);
				}
			} else {
				// 이 페이즈는 1-2 3-4 5-6 이다.
				if (this->rank != 0 && this->rank != this->process_count - 1) {
					if (this->rank % 2 == 1) {
						MPI_Send(my_segment, segment_size, MPI_INT, this->rank + 1, 0, MPI_COMM_WORLD);
						MPI_Recv(opponent_segment, segment_size, MPI_INT, this->rank + 1, 0, MPI_COMM_WORLD, &this->status);
						this->extract_small_group(my_segment, opponent_segment, segment_size, temp_segment);
						memcpy(my_segment, temp_segment, sizeof(int) * segment_size);
					} else {
						MPI_Recv(opponent_segment, segment_size, MPI_INT, this->rank - 1, 0, MPI_COMM_WORLD, &this->status);
						MPI_Send(my_segment, segment_size, MPI_INT, this->rank - 1, 0, MPI_COMM_WORLD);
						this->extract_large_group(my_segment, opponent_segment, segment_size, temp_segment);
						memcpy(my_segment, temp_segment, sizeof(int) * segment_size);
					}
				}
			}
			MPI_Barrier(MPI_COMM_WORLD);
		}

		// 이제 서로 가지고 있는 세그먼트를 모두 하나로 모은다.
		MPI_Gather(my_segment, segment_size, MPI_INT,
			this->arr, segment_size, MPI_INT,
			0, MPI_COMM_WORLD);

		// 동적 할당을 해제한다.
		if (my_segment != nullptr) {
			free(my_segment);
		}

		if (opponent_segment != nullptr) {
			free(opponent_segment);
		}

		if (temp_segment != nullptr) {
			free(temp_segment);
		}
	}
};

int main(int argc, char** argv) {
	int n = atoi(argv[1]);
	int size = n * 1024 * 1024;
	custom_array c_arr(size, argc, argv);
	double best_time = 99999.9f;
	for (int i = 0; i < 10; i++) {
		c_arr.fill_randomly(0, size);

		auto start = std::chrono::high_resolution_clock::now();
		c_arr.odd_even_sort_mpi();
		auto end = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);

		if (best_time > elapsed.count()) {
			best_time = elapsed.count();
		}
	}

	if (c_arr.get_rank() == 0) {
		if (c_arr.is_sorted() == true) {
			std::cout << "# of process: " << c_arr.get_process_count() << ", Elements: " << n << "M, Elapsed: " << best_time << " (sec)" << std::endl;
		} else {
			std::cout << "# of process: " << c_arr.get_process_count() << ", Elements: " << n << "M, Not sorted!" << std::endl;
		}
	}

	return 0;
}
