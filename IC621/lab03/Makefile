CXX=g++
TARGET=oddevensort

all:
	mpic++ $(TARGET).cpp -o $(TARGET) --std=c++11 -Wall -lpthread

run1:
	mpirun -np 1 ./$(TARGET) 2

run2:
	mpirun -np 2 ./$(TARGET) 2

run4:
	mpirun -np 4 ./$(TARGET) 2

run8:
	mpirun -np 8 ./$(TARGET) 2

test:
	mpirun -np 1 ./$(TARGET) 2
	mpirun -np 2 ./$(TARGET) 2
	mpirun -np 4 ./$(TARGET) 2
	mpirun -np 8 ./$(TARGET) 2
	mpirun -np 1 ./$(TARGET) 4
	mpirun -np 2 ./$(TARGET) 4
	mpirun -np 4 ./$(TARGET) 4
	mpirun -np 8 ./$(TARGET) 4
	mpirun -np 1 ./$(TARGET) 8
	mpirun -np 2 ./$(TARGET) 8
	mpirun -np 4 ./$(TARGET) 8
	mpirun -np 8 ./$(TARGET) 8
	mpirun -np 1 ./$(TARGET) 16
	mpirun -np 2 ./$(TARGET) 16
	mpirun -np 4 ./$(TARGET) 16
	mpirun -np 8 ./$(TARGET) 16

clean:
	rm -rf *.o
	rm -rf $(TARGET)
