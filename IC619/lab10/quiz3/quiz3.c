#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/delay.h>

#include "sbuf.h"

/* create an empty, bounded, shared FIFO buffer with n slots */
extern void sbuf_init(sbuf_t *sp, int n);

/* clean up buffer sp */
extern void sbuf_deinit(sbuf_t *sp);

/* insert item onto the rear of shared buffer sp */
extern void sbuf_insert(sbuf_t *sp, int item);

/* remove and return the first item from buffer sp */
extern int sbuf_remove(sbuf_t *sp);

#define SBUFSIZE 3
#define NUM_SBUF 1
#define NUM_THREADS 1

static struct task_struct **pthreads;
static struct task_struct **cthreads;
static int exit_flag = 0, enqueue_flag = 0, dequeue_flag = 0;
sbuf_t* sbufs = NULL;

struct workqueue_struct *my_wq;

static void work_func_esc(struct work_struct *work) {
	printk(KERN_INFO "WORKQUEUE You pressed Esc ...\n");
	exit_flag = 1;
	wake_up_process(pthreads[0]);
	wake_up_process(cthreads[0]);
	kfree(work);
}
static void work_func_f2(struct work_struct *work) {
	printk(KERN_INFO "WORKQUEUE You pressed F2 ...\n");
	enqueue_flag = 1;
	wake_up_process(pthreads[0]);
	kfree(work);
}
static void work_func_f3(struct work_struct *work) {
	printk(KERN_INFO "WORKQUEUE You pressed F3 ...\n");
	dequeue_flag = 1;
	wake_up_process(cthreads[0]);
	kfree(work);
}

irqreturn_t irq_handler(int irq, void *dev_id)
{
	static unsigned char scancode;
	unsigned char status;
	struct work_struct *my_work;

	status = inb(0x64);
	scancode = inb(0x60);
	
	switch (scancode) {
		case 0x01: 
			printk(KERN_INFO "! You pressed Esc ...\n");
			my_work = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
			INIT_WORK(my_work, work_func_esc);
			queue_work(my_wq, my_work);
			break;
		case 0x3C:
			printk(KERN_INFO "! You pressed F2 ...\n");
			my_work = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
			INIT_WORK(my_work, work_func_f2);
			queue_work(my_wq, my_work);
			break;
		case 0x3D:
			printk(KERN_INFO "! You pressed F3 ...\n");
			my_work = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
			INIT_WORK(my_work, work_func_f3);
			queue_work(my_wq, my_work);
			break;
		default:
			break;
	}

	return IRQ_HANDLED;
}


static int producer (void* arg)
{
	int idx = 0;
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (exit_flag) {
			break;
		}
		if (enqueue_flag) {
			sbuf_insert(sbufs, idx);
			printk(KERN_INFO "Producer enqueued item: %d\n", idx);
			enqueue_flag = 0;
			idx++;
		}
		schedule();
		set_current_state(TASK_RUNNING);
	}
	printk(KERN_INFO "Producer has terminated\n");
	return 0;
}

static int consumer (void* arg)
{
	int val;
	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		if (exit_flag) {
			break;
		}
		if (dequeue_flag) {
			val = sbuf_remove(sbufs);
			printk(KERN_INFO "Consumer dequeued item: %d\n", val);
			dequeue_flag = 0;
		}
		schedule();
		set_current_state(TASK_RUNNING);
	}
	printk(KERN_INFO "Consumer has terminated\n");
	return 0;
}

static int simple_init (void)
{
	request_irq(1,
			(irq_handler_t)irq_handler,
			IRQF_SHARED,
			"test_keyboard_irq_handler",
			(void *)(irq_handler));

    pthreads = (struct task_struct**)kmalloc (sizeof (struct task_struct*) * NUM_THREADS, GFP_KERNEL);
    cthreads = (struct task_struct**)kmalloc (sizeof (struct tast_struct*) * NUM_THREADS, GFP_KERNEL);
    sbufs = (sbuf_t*)kmalloc (sizeof (sbuf_t) * NUM_SBUF, GFP_KERNEL);
    sbuf_init (&sbufs[0], SBUFSIZE); 

	// create workqueue
	my_wq = create_workqueue("my_workqueue");

	pthreads[0] = kthread_run(producer, NULL, "SYS");
	cthreads[0] = kthread_run(consumer, NULL, "SYS");
	printk(KERN_INFO "! Loaded quiz3 \n");

	return 0;
}

static void simple_exit(void)
{
	flush_workqueue(my_wq); 
	destroy_workqueue(my_wq);

	sbuf_deinit(sbufs);
	free_irq(1, (void *)(irq_handler));

	kfree (pthreads);
    kfree (cthreads);
    kfree (sbufs);
	printk(KERN_INFO "! Exited quiz2 \n");
}

module_init( simple_init ); // 모듈 생성될 때 simpel_init 함수 호출
module_exit( simple_exit ); // 모듈 생성될 때 simpel_exit 함수 호출

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("KOO");
