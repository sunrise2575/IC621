all :
	g++ cpu_singlecore.cpp -o cpu_singlecore -Wall --std=c++11 -O3
	nvcc gpu_normal.cu -o gpu_normal --std=c++11 -O3
	nvcc gpu_tiled.cu -o gpu_tiled --std=c++11 -O3

test:
	./cpu_singlecore
	./gpu_normal
	./gpu_tiled

clean :
	rm -rf cpu_singlecore
	rm -rf gpu_normal
	rm -rf gpu_tiled
