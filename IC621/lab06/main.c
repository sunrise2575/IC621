#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#define BILLION 1000000000L

void fill_randomly(size_t* arr,
				   const size_t arr_size,
				   const size_t min,
				   const size_t max)
{
	srand((size_t)time(NULL));
	size_t i;
	for (i = 0; i < arr_size; i++) {
		arr[i] = ((size_t)rand()) % (max - min + 1) + min;
	}
}

void print_array(size_t* arr, const size_t arr_size) {
	size_t i;
	for (i = 0; i < arr_size; i++) {
		printf("%ld ", arr[i]);
	}
	printf("\n");
}

void odd_even_sort_static(size_t* arr, size_t arr_size, size_t chunksize) {
	int thread_count = 4;// omp_get_num_threads();

	size_t phase, i, tmp = 0;

	#pragma omp parallel \
			num_threads(thread_count) \
			default(none) \
			shared(arr, arr_size, chunksize) \
			private(i, tmp, phase)

	for (phase = 0; phase < arr_size; phase++) {
		if (phase % 2 == 0) {
			#pragma omp for \
					schedule(static, chunksize)
			for (i = 1; i < arr_size; i += 2) {
				if (arr[i-1] > arr[i]) {
					tmp = arr[i-1];
					arr[i-1] = arr[i];
					arr[i] = tmp;
				}
			}
		} else {
			#pragma omp for \
					schedule(static, chunksize)
			for (i = 1; i < arr_size-1; i += 2) {
				if (arr[i] > arr[i+1]) {
					tmp = arr[i+1];
					arr[i+1] = arr[i];
					arr[i] = tmp;
				}
			}
		}
	}
}

void odd_even_sort_dynamic(size_t* arr, size_t arr_size, size_t chunksize) {
	int thread_count = 4;// omp_get_num_threads();

	size_t phase, i, tmp = 0;

	#pragma omp parallel \
			num_threads(thread_count) \
			default(none) \
			shared(arr, arr_size, chunksize) \
			private(i, tmp, phase)

	for (phase = 0; phase < arr_size; phase++) {
		if (phase % 2 == 0) {
			#pragma omp for \
					schedule(dynamic, chunksize)
			for (i = 1; i < arr_size; i += 2) {
				if (arr[i-1] > arr[i]) {
					tmp = arr[i-1];
					arr[i-1] = arr[i];
					arr[i] = tmp;
				}
			}
		} else {
			#pragma omp for \
					schedule(dynamic, chunksize)
			for (i = 1; i < arr_size-1; i += 2) {
				if (arr[i] > arr[i+1]) {
					tmp = arr[i+1];
					arr[i+1] = arr[i];
					arr[i] = tmp;
				}
			}
		}
	}
}

void odd_even_sort_guided(size_t* arr, size_t arr_size, size_t chunksize) {
	int thread_count = 4;// omp_get_num_threads();

	size_t phase, i, tmp = 0;

	#pragma omp parallel \
			num_threads(thread_count) \
			default(none) \
			shared(arr, arr_size, chunksize) \
			private(i, tmp, phase)

	for (phase = 0; phase < arr_size; phase++) {
		if (phase % 2 == 0) {
			#pragma omp for \
					schedule(guided, chunksize)
			for (i = 1; i < arr_size; i += 2) {
				if (arr[i-1] > arr[i]) {
					tmp = arr[i-1];
					arr[i-1] = arr[i];
					arr[i] = tmp;
				}
			}
		} else {
			#pragma omp for \
					schedule(guided, chunksize)
			for (i = 1; i < arr_size-1; i += 2) {
				if (arr[i] > arr[i+1]) {
					tmp = arr[i+1];
					arr[i+1] = arr[i];
					arr[i] = tmp;
				}
			}
		}
	}
}

int main(int argc, char** argv) {
	// PARAMETER
	if (argc != 3) {
		return -1;
	}

	size_t type;
	if (!strcmp(argv[1], "s")) {
		type = 1;
	} else if (!strcmp(argv[1], "d")) {
		type = 2;
	} else if (!strcmp(argv[1], "g")) {
		type = 3;
	} else {
		printf("wrong argument!\n");
		return -1;
	}

	size_t chunksize = strtoull(argv[2], NULL, 10);

	// CREATE ARRAY
	size_t arr_size = 100000;
	size_t *arr = (size_t*)malloc(arr_size * sizeof(size_t));

	// TIME MEASURE READY
	struct timespec start, end;
	double elapsed;

	// FILL RANDOM INTEGERS
	fill_randomly(arr, arr_size, 1, BILLION);

	// STOPWATCH START
	clock_gettime(CLOCK_MONOTONIC, &start);

	// SORTING
	switch (type) {
		case 1:
			odd_even_sort_static(arr, arr_size, chunksize);
			break;
		case 2:
			odd_even_sort_dynamic(arr, arr_size, chunksize);
			break;
		case 3:
			odd_even_sort_guided(arr, arr_size, chunksize);
			break;
	}

	// STOPWATCH END
	clock_gettime(CLOCK_MONOTONIC, &end);

	// CONVERT READIBLE TIME
	elapsed = (double)(BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec)  / (double)BILLION;

	// PRINT RESULT
	switch (type) {
		case 1:
			printf("SCHEDULING: STATIC, CHUNKSIZE: %ld, ELAPSED: %8lf(sec)\n",
				chunksize, elapsed);
			break;

		case 2:
			printf("SCHEDULING: DYNAMIC, CHUNKSIZE: %ld, ELAPSED: %8lf(sec)\n",
				chunksize, elapsed);
			break;

		case 3:
			printf("SCHEDULING: GUIDED, CHUNKSIZE: %ld, ELAPSED: %8lf(sec)\n",
				chunksize, elapsed);
			break;
	}

	// ODD-EVEN SORT
	free(arr);

	return 0;
}
