all:
	gcc main.c -o main -O3 -Wall -fopenmp -lrt

test:
	./main s 1
	./main s 100
	./main s 10000
	./main d 1
	./main d 100
	./main d 10000
	./main g 1
	./main g 100
	./main g 10000

clean:
	rm -rf main