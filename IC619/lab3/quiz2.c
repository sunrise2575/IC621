#include <stdio.h>
#include <sys/syscall.h>

int main() {
	long int a;
	a = syscall(334, '+', 100, 2);
	printf("100 + 2 = %d\n", a);
	a = syscall(334, '-', 23, 8);
	printf("23 - 8 = %d\n", a);
	a = syscall(334, '*', 3, 28);
	printf("3 * 28 = %d\n", a);
	a = syscall(334, '/', 99, 11);
	printf("99 / 11 = %d\n", a);
	return 0;
}
