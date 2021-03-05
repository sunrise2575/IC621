#include <stdio.h>
#include <sys/syscall.h>

int main(void) {
	long int a = syscall(333);
	printf("System Call returned %ld\n", a);
	return 0;
}
