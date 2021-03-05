#include <linux/kernel.h>

asmlinkage long sys_quiz1(void) {
	printk(KERN_INFO "sys_quiz1 has been added\n");
	return 0;
}
