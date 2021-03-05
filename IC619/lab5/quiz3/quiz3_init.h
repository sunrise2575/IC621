#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/hashtable.h>

#define NODE_SIZE 10
#define FALSE 0
#define TRUE 1

typedef struct rbnode{
    struct rb_node node;
    char keystring;
}my_node;

extern my_node *data_node;
extern struct rb_root my_root;

my_node* rb_search(struct rb_root *root, char string);
int rb_insert(struct rb_root *root, my_node *data);
void create(void);
void iterate(void);
void destroy(void);
