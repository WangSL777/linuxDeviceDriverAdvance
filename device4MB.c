#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#define MAJOR_NUMBER 62
#define DEVICE_MAX_SIZE 34

/* forward declaration */
int device4MB_open(struct inode *inode, struct file *filep);
int device4MB_release(struct inode *inode, struct file *filep);
ssize_t device4MB_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t device4MB_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
static void device4MB_exit(void);

/* definition of file_operation structure */
struct file_operations device4MB_fops = 
{
	read: device4MB_read,
	write: device4MB_write,
	open: device4MB_open,
	release: device4MB_release
};

char *device4MB_data = NULL;

int device4MB_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int device4MB_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

ssize_t device4MB_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{

	int i;
	char nop[1] ={0};
	if((*f_pos) > 0)
		return 0; //end of file, this will stop continously print messge

	i = copy_to_user(buf, device4MB_data, DEVICE_MAX_SIZE);
	if(i<DEVICE_MAX_SIZE)
		copy_to_user(&buf[i], nop, 1);

	(*f_pos) ++;
	return DEVICE_MAX_SIZE;
}

ssize_t device4MB_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
/*
	if((*f_pos) > (DEVICE_MAX_SIZE-1))
		return -ENOSPC;//throw "No Space left on device" error
	else
	{
		device4MB_data[(*f_pos)] = buf[(*f_pos)];//copy from user data 
		(*f_pos)++;
		return 1;
	}
*/
	int i;
	if(count>DEVICE_MAX_SIZE)
		return -ENOSPC;

	for(i = 0; i<count; i++ )
	{
		device4MB_data[i] = buf[i];
	}
	return i;

}

static int device4MB_init(void)
{
	int result;
	// register the device
	result = register_chrdev(MAJOR_NUMBER, "device4MB", &device4MB_fops);
	if (result < 0) 
	{
		return result;
	}

	// allocate 4MB byte of memory for storage
	// kmalloc is just like malloc, the second parameter is
	// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.
	device4MB_data = kmalloc(sizeof(char)*DEVICE_MAX_SIZE, GFP_KERNEL);

	if (!device4MB_data) 
	{
		device4MB_exit();
	// cannot allocate memory
	// return no memory error, negative signify a failure
		return -ENOMEM;
	}
	// initialize the value to be X
	sprintf(device4MB_data,"This is a device4MB device module");
	printk(KERN_ALERT "This is a device4MB device module\n");
	return 0;
}

static void device4MB_exit(void)
{
	// if the pointer is pointing to something
	if (device4MB_data) {
	// free the memory and assign the pointer to NULL
	kfree(device4MB_data);
	device4MB_data = NULL;
	}
	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "device4MB");
	printk(KERN_ALERT "device4MB device module is unloaded\n");
}
MODULE_LICENSE("GPL");
module_init(device4MB_init);
module_exit(device4MB_exit);
