/*
 *  chardev.c - Create an input/output character device
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/fs.h>
#include <asm/uaccess.h>	/* for get_user and put_user */
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/mman.h>

#include "chardev.h"
#define SUCCESS 0
#define DEVICE_NAME "char_dev"

/* 
 * Is the device open right now? Used to prevent
 * concurent access into the same device 
 */
static int Device_Open = 0;

/* 
 * This is called whenever a process attempts to open the device file 
 */
static int device_open(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_open(%p)\n", file);
#endif

	/* 
	 * We don't want to talk to two processes at the same time 
	 */
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	/*
	 * Initialize the message 
	 */
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
#ifdef DEBUG
	printk(KERN_INFO "device_release(%p,%p)\n", inode, file);
#endif

	/* 
	 * We're now ready for our next caller 
	 */
	Device_Open--;

	module_put(THIS_MODULE);
	return SUCCESS;
}

/* 
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */
long device_ioctl(struct file *file,	/* see include/linux/fs.h */
		unsigned int ioctl_num,	/* number and param for ioctl */
		unsigned long ioctl_param)
{
	struct vm_area_struct *vmlist;
	unsigned long size = 0;
	switch (ioctl_num) {
	case IOCTL_VMAINFO:
		printk(KERN_INFO "%d: %s\n", current->pid, current->comm);
		for (vmlist = current->mm->mmap; vmlist != NULL; vmlist = vmlist->vm_next) {
			char filename[50], flag[4];

			if (vmlist->vm_file) {
				strcpy(filename, vmlist->vm_file->f_path.dentry->d_iname);
			} else {
				strcpy(filename, " [ anon ]");
			}

			flag[0] = (vmlist->vm_flags & VM_READ)  ? 'r' : '-';
			flag[1] = (vmlist->vm_flags & VM_WRITE) ? 'w' : '-';
			flag[2] = (vmlist->vm_flags & VM_EXEC)  ? 'x' : '-';
			flag[3] = '\0'; 

			printk(KERN_INFO "%016lx %6ldK %s %s\n",
					vmlist->vm_start,
					(vmlist->vm_end - vmlist->vm_start) / 1024,
					flag,
					filename);
			size += vmlist->vm_end - vmlist->vm_start;
		}

		printk(KERN_INFO " total VMA address space %6ldK\n", size / 1024);

		vmlist = find_vma(current->mm, current->mm->start_code);
		printk(KERN_INFO "code  %016lx ~ %016lx %8ldK\n", vmlist->vm_start, vmlist->vm_end, (vmlist->vm_end - vmlist->vm_start) / 1024);
		vmlist = find_vma(current->mm, current->mm->start_data);
		printk(KERN_INFO "data  %016lx ~ %016lx %8ldK\n", vmlist->vm_start, vmlist->vm_end, (vmlist->vm_end - vmlist->vm_start) / 1024);
		vmlist = find_vma(current->mm, current->mm->start_brk);
		printk(KERN_INFO "heap  %016lx ~ %016lx %8ldK\n", vmlist->vm_start, vmlist->vm_end, (vmlist->vm_end - vmlist->vm_start) / 1024);
		vmlist = find_vma(current->mm, current->mm->start_stack);
		printk(KERN_INFO "stack %016lx ~ %016lx %8ldK\n", vmlist->vm_start, vmlist->vm_end, (vmlist->vm_end - vmlist->vm_start) / 1024);
		break;
	}

	return SUCCESS;
}

/* Module Declarations */

/* 
 * This structure will hold the functions to be called
 * when a process does something to the device we
 * created. Since a pointer to this structure is kept in
 * the devices table, it can't be local to
 * init_module. NULL is for unimplemented functions. 
 */
struct file_operations Fops = {
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,	/* a.k.a. close */
};

/* 
 * Initialize the module - Register the character device 
 */
int init_module()
{
	int ret_val;
	/* 
	 * Register the character device (atleast try) 
	 */
	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &Fops);

	/* 
	 * Negative values signify an error 
	 */
	if (ret_val < 0) {
		printk(KERN_ALERT "%s failed with %d\n",
				"Sorry, registering the character device ", ret_val);
		return ret_val;
	}

	printk(KERN_INFO "%s The major device number is %d.\n",
			"Registeration is a success", MAJOR_NUM);
	printk(KERN_INFO "If you want to talk to the device driver,\n");
	printk(KERN_INFO "you'll have to create a device file. \n");
	printk(KERN_INFO "We suggest you use:\n");
	printk(KERN_INFO "mknod %s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
	printk(KERN_INFO "The device file name is important, because\n");
	printk(KERN_INFO "the ioctl program assumes that's the\n");
	printk(KERN_INFO "file you'll use.\n");

	return 0;
}

/* 
 * Cleanup - unregister the appropriate file from /proc 
 */
void cleanup_module()
{
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

#if 0
	int ret;

	/* 
	 * Unregister the device 
	 */
	ret = unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

	/* 
	 * If there's an error, report it 
	 */
	if (ret < 0)
		printk(KERN_ALERT "Error: unregister_chrdev: %d\n", ret);
#endif
}
