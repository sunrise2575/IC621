#include <linux/kernel.h>
#include <linux/string.h>
#define __QUIZ3_BUF_SIZE__ 4

asmlinkage int sys_quiz3(int a, int b, char c) {
	int i, j, count = 0;
	printk(KERN_INFO "sys_quiz3 has been added\n");
	for (i = a; i <= b; i++) {
		char buf[__QUIZ3_BUF_SIZE__] = {0,};
		snprintf(buf, __QUIZ3_BUF_SIZE__, "%d", i);
		for (j = 0; j < __QUIZ3_BUF_SIZE__; j++) {
			if (buf[j] == c) {
				count++;
			}
		}
	}
	return count;
}
