// C++ headers
#include <iostream>
#include <chrono>

// C headers
#include <cstdlib>
#include <errno.h>

// POSIX thread headers
#include <pthread.h>

// node
struct node_t {
	pthread_mutex_t mutex;
	int data;
	node_t* next;
};

static node_t* head = nullptr;
static size_t threads = 1;
static size_t elements = 100;

void* list_worker(void* rank); // pthread function
void list_insert(int new_value); // pthread subroutine
void list_print(); // print list

int main(int argc, char** argv) {
    if (argc != 3) {
        exit(EXIT_FAILURE);
    }

    elements = (size_t)strtol(argv[1], nullptr, 10);
    threads = (size_t)strtol(argv[2], nullptr, 10);

    // head를 동적 할당한다.
	head = (node_t*)malloc(sizeof(node_t));
    if (!head) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // head의 내용을 채워넣는다.
	pthread_mutex_init(&head->mutex, nullptr);
	head->data = 1;
	head->next = nullptr;

    // 쓰레드 갯수와
    pthread_t* thread_handles = (pthread_t*)malloc(sizeof(pthread_t) * threads);

    auto start = std::chrono::high_resolution_clock::now();

	for (size_t i = 0; i < threads; i++) {
		pthread_create(&thread_handles[i], nullptr, list_worker, (void*)i);
	}

	for (size_t i = 0; i < threads; i++) {
		pthread_join(thread_handles[i], nullptr);
	}

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double> >(end - start);

	list_print();

    std::cout << "elements: " << elements << ", threads: " << threads << ", elapsed: " << elapsed.count() << "(sec)" << std::endl;

	return 0;
}

void* list_worker(void* rank) {
	size_t my_rank = (size_t)rank;
	for (auto i = my_rank + 2; i < elements + 1; i += threads) {
		list_insert(i);
	}

	return nullptr;
}

void list_insert(int new_value) {
    // head에 lock을 잡는다.
	pthread_mutex_lock(&head->mutex);

    // 일단 현재 위치를 head로, 다음 위치는 없는 것으로 한다.
	node_t* current = head;
	node_t* previous = nullptr;

    // 내용이 들어갈 적절한 위치 찾기
	while (true){
		if (current->next != nullptr) {
			pthread_mutex_lock(&current->next->mutex);

			if (current->next->data < new_value) {
				previous = current;
				current = current->next;
				pthread_mutex_unlock(&previous->mutex);
			} else {
				pthread_mutex_unlock(&current->next->mutex);
				break;
			}
		} else {
			break;
		}
	}

    // 새 node를 넣기 위해 malloc
	node_t* inserted = (node_t*)malloc(sizeof(node_t));
    if (!inserted) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // 새 node에 내용 채워넣기
	pthread_mutex_init(&inserted->mutex, nullptr);
	inserted->data = new_value;
	inserted->next = current->next;

    // 넣은 내용을 이전의 링크드 리스트와 연결시키기
	current->next = inserted;
	pthread_mutex_unlock(&current->mutex);
}

void list_print() {
    for (auto current = head; current != nullptr; current = current->next) {
		std::cout << current->data << " ";
	}
    std::cout << std::endl;
}
