#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


static int myshort = 1;
static int myint = 420;
static long int mylong = 9999;
static char *mystring = "System Programming";
static int myintArray[10] = {0,};

static int arr_argc = 0;
module_param(myint, int, 0);
module_param_array(myintArray, int, &arr_argc, 0);

static int quiz2_init(void) {
	int i;
	printk(KERN_INFO "Loading Moudule\n");
	printk(KERN_INFO "Quiz 2\n=====================\n");
	printk(KERN_INFO "myshort is a short integer: %d\n", myshort);
	printk(KERN_INFO "myint is an integer: %d\n", myint);
	printk(KERN_INFO "mylong is a long integer: %d\n", mylong);
	printk(KERN_INFO "mystring is a string: %s\n", mystring);
	for (i = 0; i < 10; i++) { printk(KERN_INFO "myintArray[%d] = %d\n", i, myintArray[i]); }
	printk(KERN_INFO "got %d arguments for myintArray\n", arr_argc);
	return 0;
}

static void quiz2_exit(void) {
	int i, j;
	printk(KERN_INFO "Removing Module\n");
	printk(KERN_INFO "Sort Result\n");
	printk(KERN_INFO "Original myintArray\n");
	for (i = 0; i < 10; i++) { printk(KERN_INFO "myintArray[%d] = %d\n", i, myintArray[i]); }

	for (i = 0; i < 9; i++) {
		int temp = 0;
		for (j = 0; j < 9; j++) {
			if (myintArray[j] >= myintArray[j+1]) {
				temp = myintArray[j];
				myintArray[j] = myintArray[j+1];
				myintArray[j+1] = temp;
			}
		}
	}

	printk(KERN_INFO "Sorted myintArray\n");
	for (i = 0; i < 10; i++) { printk(KERN_INFO "myintArray[%d] = %d\n", i, myintArray[i]); }
}

module_init( quiz2_init );
module_exit( quiz2_exit );

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quiz2 Module");
MODULE_AUTHOR("18SYS");
