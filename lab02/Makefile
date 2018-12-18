all:
	mpicc -g -Wall -o mpi_hello mpi_hello.c
	mpicc -g -Wall -o mpi_trap mpi_trap.c
	mpicc -g -Wall -o mpi_getinput mpi_getinput.c

run:
	mpiexec -n 4 ./mpi_hello
	mpiexec -n 4 ./mpi_trap
	mpiexec -n 4 ./mpi_getinput

run_hello:
	mpiexec -n 4 ./mpi_hello

run_trap:
	mpiexec -n 4 ./mpi_trap

run_getinput:
	mpiexec -n 4 ./mpi_getinput

clean:
	rm -rf mpi_hello
	rm -rf mpi_trap
	rm -rf mpi_getinput