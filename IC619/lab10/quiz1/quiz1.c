/*
 *   irq_ex1.c: An interrupt handler example. This code binds itself to `IRQ` 1, which
 *   is the IRQ of the keyboard controlled under Intel architectures. Then, when it
 *   receives a keyboard interrupt, it reads the keyboard's status and the scan code,
 *   which is the value return by the keyboard. And then, it puts information about
 *   Key that pressed. This example only has 3 key: ESC, F1 and F2
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("18SYS");

irqreturn_t irq_handler(int irq, void *dev_id)
{
	static unsigned char scancode;
	unsigned char status;

	status = inb(0x64);
	scancode = inb(0x60);
	
	switch (scancode) {
		case 0x01: printk(KERN_INFO "! You pressed Esc ...\n"); break;
		case 0x3C: printk(KERN_INFO "! You pressed F2 ...\n"); break;
		case 0x3D: printk(KERN_INFO "! You pressed F3 ...\n"); break;
		default: break;
	}

	return IRQ_HANDLED;
}

/*
 * Initialize the module - register the IRQ handler
 */
static int __init irq_ex_init(void)
{
	printk(KERN_INFO "! Loaded quiz1 \n");
	return request_irq(1,
			(irq_handler_t)irq_handler,
			IRQF_SHARED,
			"test_keyboard_irq_handler",
			(void *)(irq_handler));
}

static void __exit irq_ex_exit(void)
{
	free_irq(1, (void *)(irq_handler));
	printk(KERN_INFO "! Exited quiz1 \n");
}

module_init(irq_ex_init);
module_exit(irq_ex_exit);
