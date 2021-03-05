#include <stdio.h>
#include <sys/syscall.h>

int main() {
	long int a;
	a = syscall(335, 20, 30, '2');
	printf("%d\n", a);
	a = syscall(335, 11, 15, '1');
	printf("%d\n", a);
	a = syscall(335, 6, 100, '8');
	printf("%d\n", a);
	return 0;
}
