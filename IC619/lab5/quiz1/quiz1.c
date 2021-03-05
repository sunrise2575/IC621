#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>

static LIST_HEAD(movie_list);

struct movie {
	unsigned int year;
	unsigned int episode;
	char title[128];
	struct list_head list;
};

static void add_element(unsigned int _year, unsigned int _episode, const char* _title) {
	struct movie *my_movie = kmalloc(sizeof(*my_movie), GFP_KERNEL);
	my_movie->year = _year;
	my_movie->episode = _episode;
	strcpy(my_movie->title, _title);
	INIT_LIST_HEAD(&my_movie->list);
	list_add_tail(&my_movie->list, &movie_list); 
}

static void print_my_element(void) {
	struct list_head* my_iter;
	list_for_each(my_iter, &movie_list) {
		struct movie* obj = list_entry(my_iter, struct movie, list);
		printk(KERN_INFO "Opening year : %d, Title : %s, Episode number : %d\n",
			obj->year, obj->title, obj->episode);
	}
}

int kernel_init(void){
	add_element(1997, 4, "Star Wars");
	add_element(1980, 5, "The Empire Strikes Back");
	add_element(1983, 6, "Return of the Jedi");
	add_element(1999, 1, "Star wars: Episode I – The Phantom Menace");
	add_element(2002, 2, "Star Wars: Episode II – Attack of the Clones");
	add_element(2005, 3, "Star Wars: Episode III – Revenge of the Sith");
	add_element(2015, 7, "Star Wars: The Force Awakens");
	add_element(2017, 8, "Star wars: The Last Jedi");
	print_my_element();
	return 0;
}

void kernel_exit(void){
	struct list_head *my_iter, *temp;
	list_for_each_safe(my_iter, temp, &movie_list) {
		struct movie* obj = list_entry(my_iter, struct movie, list);
		printk(KERN_INFO "Delete episode : %d\n", obj->episode);
		kfree(obj);
		list_del(&obj->list);
	}
	printk(KERN_INFO "All list delete");
}

module_init(kernel_init);
module_exit(kernel_exit);

MODULE_LICENSE("GPL");
