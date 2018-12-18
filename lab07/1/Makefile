CXX = g++
CXXFLAGS = -g -Wall -O2 --std=c++11 -lpthread -lrt
TARGET = static

all:
	$(CXX) $(TARGET).cpp -o $(TARGET) $(CXXFLAGS)

test:
	./$(TARGET) 1
	./$(TARGET) 2
	./$(TARGET) 4
clean:
	rm $(TARGET)
