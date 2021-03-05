#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

#define MS_TO_NS(x) (x * 1E6L)

unsigned long timer_interval = MS_TO_NS(5000);
static struct hrtimer hr_timer;

enum hrtimer_restart my_hrtimer_callback(struct hrtimer *my_timer) {
	ktime_t currtime, interval;
	currtime = ktime_get();
	interval = ktime_set(0, timer_interval);
	hrtimer_forward(my_timer, currtime, interval);
	printk(KERN_INFO "Call timer!");
	return HRTIMER_RESTART;
}

static int __init timer_module(void){
	ktime_t ktime = ktime_set(0, timer_interval);
	hrtimer_init(&hr_timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	hr_timer.function = &my_hrtimer_callback;
	hrtimer_start(&hr_timer, ktime, HRTIMER_MODE_REL);
	printk(KERN_INFO "Start timer...");

    return 0;
}

static void __exit clean_up(void){
	int ret = hrtimer_cancel(&hr_timer);
	if (ret) printk(KERN_INFO "The timer was still in use...\n");
	printk(KERN_INFO "Finish timer...");

    return;
}

module_init(timer_module);
module_exit(clean_up);
