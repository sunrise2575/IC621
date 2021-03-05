#include <linux/kernel.h>

asmlinkage int sys_quiz2(char operator, int num1, int num2) {
	printk(KERN_INFO "sys_quiz2 has been added\n");
	if (operator == '+') { return num1 + num2; }
	if (operator == '-') { return num1 - num2; }
	if (operator == '*') { return num1 * num2; }
	if (operator == '/') { return num1 / num2; }
	return 0;
}
