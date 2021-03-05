#include <linux/kernel.h>

asmlinkage long sys_fact(int n) {
	int value = 0;
	if (n == 1) return n;
	value = n * sys_fact(n-1);
	return value;
}
