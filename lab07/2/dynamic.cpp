// Heeyong Yoon
// 201411096

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>

#define BILLION 1000000000L // For time measuring

// Global variables
static constexpr int NOCITY = INT_MAX;
static constexpr int CITY_COUNT = 15;
static constexpr int matrix[CITY_COUNT][CITY_COUNT] = {
    { 0, 83, 86, 77, 15, 93, 35, 86, 92, 49, 21, 62, 27, 90, 59},
    {63,  0, 26, 40, 26, 72, 36, 11, 68, 67, 29, 82, 30, 62, 23},
    {67, 35,  0, 29,  2, 22, 58, 69, 67, 93, 56, 11, 42, 29, 73},
    {21, 19, 84,  0, 37, 98, 24, 15, 70, 13, 26, 91, 80, 56, 73},
    {62, 70, 96, 81,  0,  5, 25, 84, 27, 36,  5, 46, 29, 13, 57},
    {24, 95, 82, 45, 14,  0, 67, 34, 64, 43, 50, 87,  8, 76, 78},
    {88, 84,  3, 51, 54, 99,  0, 32, 60, 76, 68, 39, 12, 26, 86},
    {94, 39, 95, 70, 34, 78, 67,  0,  1, 97,  2, 17, 92, 52, 56},
    { 1, 80, 86, 41, 65, 89, 44, 19,  0, 40, 29, 31, 17, 97, 71},
    {81, 75,  9, 27, 67, 56, 97, 53, 86,  0, 65,  6, 83, 19, 24},
    {28, 71, 32, 29,  3, 19, 70, 68,  8, 15,  0, 40, 49, 96, 23},
    {18, 45, 46, 51, 21, 55, 79, 88, 64, 28, 41,  0, 50, 93, 99},
    {34, 64, 24, 14, 87, 56, 43, 91, 27, 65, 59, 36,  0, 32, 51},
    {37, 28, 75,  7, 74, 21, 58, 95, 29, 37, 35, 93, 18,  0, 28},
    {43, 11, 28, 29, 76,  4, 43, 63, 13, 38,  6, 40,  4, 18,  0},
};

// Results for each threads
static int best_cost[CITY_COUNT];
static int best_path[CITY_COUNT][CITY_COUNT + 1];

// Global variables initialized on the beginning
static int thread_count;

// Critical variables
static int processing[CITY_COUNT] = {0, };
static pthread_mutex_t cas_mutex[CITY_COUNT] = {PTHREAD_MUTEX_INITIALIZER, };

// Compare and swap using pthread mutex
int compare_and_swap(int* value, int city, int expected, int new_value) {
    pthread_mutex_lock(&cas_mutex[city]);
    int temp = *value;
    if (*value == expected) {
        *value = new_value;
    }
    pthread_mutex_unlock(&cas_mutex[city]);
    return temp;
}

// Find minimum cost of "i -> (other city)"
int minimum(int i) {
    int result = INT_MAX;
    for (int j = 0; j < CITY_COUNT; j++) {
        if (matrix[i][j] < result && i != j) {
            result = matrix[i][j];
        }
    }
    return result;
}

// Recursive function running on a thread
// Using Branch-and-Bound(B&B) technique
void TSP_RECURSIVE(int bound, int cost, const int level, int* path, bool* visited, const int city_of_duty) {
    // Last level of the tree (=leaf)
    if (level == CITY_COUNT) {
        int cost_to_zero = matrix[path[level - 1]][path[0]];
        // Change best_path if lower cost was found
        if (cost_to_zero && best_cost[city_of_duty] > cost + cost_to_zero) {
            for (int i = 0; i < CITY_COUNT; i++) {
                best_path[city_of_duty][i] = path[i];
            }
            best_path[city_of_duty][CITY_COUNT] = path[0];
            best_cost[city_of_duty] = cost + cost_to_zero;
        }
        // Move back to former level (note: it is recursive)
        return;
    }

    // Access to all reachable city
    for (int i = 0; i < CITY_COUNT; i++) {
        // filter current city and visited city
        if (matrix[path[level - 1]][i] != 0 && visited[i] == false) {
            // keep current bound and
            // assume that we're going to next level and estimate next bound
            int temp = bound;
            cost += matrix[path[level - 1]][i];
            bound -= minimum(i);

            // If the sum of bound and current cost is lower than current best cost
            // go deep down to the next level (recursive call)
            if (bound + cost < best_cost[city_of_duty]) {
                path[level] = i;
                visited[i] = true;
                TSP_RECURSIVE(bound, cost, level + 1, path, visited, city_of_duty);
            }

            // if recursive function is over or wasn`t excuted, then organize numerical mess
            cost -= matrix[path[level - 1]][i];
            bound = temp;

            // To not to visit already visited cities on next For loop
            // Check all the cities which I've already traveled
            memset(visited, false, sizeof(visited) * CITY_COUNT);
            for (int j = 0; j <= level - 1; j++) {
                visited[path[j]] = true;
            }
        }
    }
}

// Thread function
void* TSP_THREADED(void* arg) {
    long rank = (long)arg;

    // Calculate initial bound
    int curr_bound = 0;
    for (int i = 0; i < CITY_COUNT; i++) {
        curr_bound += minimum(i);
    }
	
	// Dynamic schedule using atomic compare and swap
    for (int i = 1 + (int)rank; i < CITY_COUNT; i++) {
        if (!compare_and_swap(&processing[i], i, 0, 1)) {
			int curr_path[CITY_COUNT + 1];
        	memset(curr_path, -1, sizeof(curr_path));
        	bool* visited = new bool[CITY_COUNT];
        	memset(visited, false, sizeof(visited) * CITY_COUNT);

        	visited[0] = true;
        	visited[i] = true;
        	curr_path[0] = 0;
        	curr_path[1] = i;

        	TSP_RECURSIVE(curr_bound, matrix[0][i], 2, curr_path, visited, i);
		}
	}

    return nullptr;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <#_of_threads>\n", argv[0]);
        return 0;
    }
    thread_count = atoi(argv[1]);

    for (int i = 0; i < CITY_COUNT; i++) {
        best_cost[i] = INT_MAX;
    }

    printf("Threads: %d (dynamic)\n", thread_count);

    struct timespec start, end;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    /////////////////// Function start ///////////////////
    pthread_t* thread_handle = new pthread_t[thread_count];

    // Launch threads
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&thread_handle[i], nullptr, &TSP_THREADED, (void*)(long)i);
    }

    // Join threads
    for (int i = 0; i < thread_count; i++) {
        pthread_join(thread_handle[i], (void**)nullptr);
    }

    // Reduction
    int best_pos = 0;
    int current_best = INT_MAX;
    for (int i = 1; i < CITY_COUNT; i++) {
        if (best_cost[i] < current_best) {
            current_best = best_cost[i];
            best_pos = i;
        }
    }

    delete[] thread_handle;
    /////////////////// Function End ///////////////////

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (double)(BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec) / (double)BILLION;

    // Print Results
    printf("   Minimum Cost : %d\n", best_cost[best_pos]);
    printf("   Minimum Path : ");
    for (int i = 0; i <= CITY_COUNT; i++) {
        printf("%d", best_path[best_pos][i]);
        if (i != CITY_COUNT) {
            printf("->");
        }
    }
    printf("\n");
    printf("   Elapsed Time : %8lf\n", elapsed);

    return 0;
}
