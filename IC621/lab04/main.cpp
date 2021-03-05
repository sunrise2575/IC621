// C headers
#include <cstdlib>
#include <ctime>
#include <cstring> //memset

// C++ headers
#include <iostream>
#include <random>

// pthread
#include <pthread.h>

#define SEED_CHANGE_PERIOD 100000
#define BILLION 1000000000L

void* monte_carlo_pi(void* param) {
	long long number_of_tosses = (long long)param;

	double x, y;
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<double> dist(-1, 1);

	long long number_in_circle, i;
	for (i = 0, number_in_circle = 0; i < number_of_tosses; i++) {
		if (i % SEED_CHANGE_PERIOD == 0) {
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			gen.seed(BILLION * ts.tv_sec + ts.tv_nsec);
		}

		// x = (2 * ((double)rand() / RAND_MAX)) - 1;
		// y = (2 * ((double)rand() / RAND_MAX)) - 1;
		x = dist(gen);
		y = dist(gen);

		if (x * x + y * y < 1) {
			number_in_circle++;
		}
	}

	return (void*)number_in_circle;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		exit(-1);
	}

	// monte carlo variables
	double pi = 0.0;
	long long number_of_tosses = strtoll(argv[1], NULL, 10);

	// thread reduce
	long long* gathered_number_in_circle = NULL;
	long long total_number_in_circle = 0;

	// thread variables
	pthread_t* thread_handle = NULL;
	int thread_count = atoi(argv[2]);
	int t;

	// time variables
	struct timespec start, end;
	double elapsed;

	// dynamic allocate thread handle array
	thread_handle = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	memset(thread_handle, 0x00, thread_count * sizeof(pthread_t));

	// dynamic allocate gather array
	gathered_number_in_circle = (long long*)malloc(thread_count * sizeof(long long));
	memset(gathered_number_in_circle, 0x00, thread_count * sizeof(long long));

	// launch
	for (t = 0; t < thread_count; t++) {
		pthread_create(&thread_handle[t], NULL, &monte_carlo_pi, (void*)(number_of_tosses/thread_count));
	}

	// time measure start
	clock_gettime(CLOCK_MONOTONIC, &start);

	// join
	for (t = 0; t < thread_count; t++) {
		pthread_join(thread_handle[t], (void**)&gathered_number_in_circle[t]);
	}

	// reduce number_in_circle to gather array
	for (t = 0; t < thread_count; t++) {
		total_number_in_circle += gathered_number_in_circle[t];
	}

	// calculate pi
	pi = 4 * ((double)total_number_in_circle / (double)number_of_tosses);

	// time measure stop
	clock_gettime(CLOCK_MONOTONIC, &end);

	// calculate elapsed time and display results
	elapsed = (double)(BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec) / (double)BILLION;
	printf("CIRCLE/TOSS: %11lld / %11lld, PI: %8lf, PROCESS: %2d, ELAPSED: %8lf(sec)\n",
		total_number_in_circle, number_of_tosses, pi, thread_count, elapsed);

	// deallocate
	if (gathered_number_in_circle != NULL) {
		free(gathered_number_in_circle);
	}

	if (thread_handle != NULL) {
		free(thread_handle);
	}

	return 0;
}
