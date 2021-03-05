#include <stdio.h>

int factorial(int n) {
	int value=0;
	if (n == 1)
		return n;
	value = n * factorial(n-1);
	return value;
}

int main() {
	int i;
	int ret =0;
	for (i = 1; i <= 10; i++) {
		ret = factorial(i);
		printf("%d! == %d\n", i, ret);
	}
	return 0;
}
