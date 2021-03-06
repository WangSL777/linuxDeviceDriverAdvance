#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#include "device4MB.h"

#define MAJOR_NUMBER 62
#define DEVICE_MAX_SIZE 4194304

/* forward declaration */
int device4MB_open(struct inode *inode, struct file *filep);
int device4MB_release(struct inode *inode, struct file *filep);
ssize_t device4MB_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t device4MB_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
loff_t device4MB_llseek(struct file* filp, loff_t offset, int whence);
static void device4MB_exit(void);
long device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

/* definition of file_operation structure */
struct file_operations device4MB_fops = 
{
	read: device4MB_read,
	write: device4MB_write,
	open: device4MB_open,
	release: device4MB_release,
	llseek: device4MB_llseek,
	.unlocked_ioctl = device_ioctl
};

char *device4MB_data = NULL;
int startOffset = 0;
int endOffset = -1;
char * dev_msg = NULL;

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
	int num = 0;
	int retval = 0;
	int availNum = 0;
	
	if(endOffset<0 ||(filep->f_pos) >= (endOffset + 1) || (filep->f_pos) <0 ||(*f_pos) >= (endOffset + 1))
		return 0; //end of file, this will stop continously print messge
	
	availNum = endOffset - (filep->f_pos) + 1;

	if(count<=0 || availNum <=0)
		return 0;
	
	if(count<=availNum)
		num = count;
	else
		num = availNum;

	retval = copy_to_user(buf, device4MB_data, num);
	if(retval!=0)
		return -EFAULT;

	(filep->f_pos) += num;
	(*f_pos) += num;
	
	printk(KERN_ALERT "device4MB_read read %d byte from device\n", num);
	return num;
}

ssize_t device4MB_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	int num,i,availNum;

	if(endOffset == -1)
	{
		availNum = DEVICE_MAX_SIZE;
	}
	else
	{
		if( (filep->f_pos)>endOffset || (filep->f_pos) <0)
			return 0;

		availNum = endOffset - (filep->f_pos) + 1;
	}

	
	if(count <= 0)
		return 0;
	else if(count > 0 && count <= availNum)
	{
		num = count;
		if(endOffset == -1)
			endOffset = num -1;
	}
	else if(count > availNum && count <= (DEVICE_MAX_SIZE - (filep->f_pos)))
	{
		num = count;
		endOffset = (filep->f_pos) + num -1;
	}
	else
	{
		num = (DEVICE_MAX_SIZE - (filep->f_pos));
		endOffset = DEVICE_MAX_SIZE -1;
	}

	for(i = 0; i<num; i++ )
	{
		device4MB_data[(filep->f_pos)+i] = buf[i];
	}
	(filep->f_pos) += num;

	printk(KERN_ALERT "device4MB_write write %d byte to device\n", i);
	return i;
}

loff_t device4MB_llseek(struct file* filp, loff_t offset, int whence)
{
	loff_t new_pos = 0;
	switch(whence)
	{
		case SEEK_SET: /* 0: */
			new_pos = offset;
			break;
		case SEEK_CUR: /* 1: */
			new_pos = filp->f_pos + offset;
			break;
		case SEEK_END: /* 2: */
			new_pos = endOffset + offset +1;
			break;
		default: /* not supported */
			printk(KERN_ALERT "llseek, not support whence, return %d\n",(int)(-EINVAL));	
			return -EINVAL;
	}
	if(new_pos < 0)
		new_pos = 0;
	if(new_pos >= (endOffset + 1))
		new_pos = endOffset + 1;
	filp->f_pos = new_pos;	

	printk(KERN_ALERT "device4MB_llseek move file pointer to offset %d\n", (int)new_pos);
	return new_pos;
}

long device_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int ret = 0;
	char * temp = NULL;
	int tempret1 = 0;
	int tempret2 = 0;
	int i=0;
	
	if(_IOC_TYPE(cmd) != DEVICE4MB_IOC_MAGIC)
		return -EINVAL;
	if(_IOC_NR(cmd) > DEVICE4MB_IOC_MAXNR)
		return -EINVAL;

	if(_IOC_DIR(cmd) & _IOC_READ)
	{
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	}
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
	{
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	}

	if(err)
		return -EFAULT;

	switch(cmd)
	{
		case DEVICE4MB_IOCHELLO:
			printk(KERN_WARNING "Hello ioctl\n");
			break;
		case DEVICE4MB_IOCGET:
			if(dev_msg)
			{
				if(copy_to_user((char *)arg, dev_msg, strlen(dev_msg))==0);
					ret = strlen(dev_msg);
			}
			break;
		case DEVICE4MB_IOCSET:
			if(dev_msg)
			{
				kfree(dev_msg);
				dev_msg = NULL;
			}
			dev_msg = kmalloc(strlen((char *)arg)+1, GFP_KERNEL);
			for(i=0;i<strlen((char *)arg);i++)
				dev_msg[i] = 0;
			if(copy_from_user(dev_msg, (char *)arg, strlen((char *)arg))==0)
				ret = strlen((char *)arg);
			break;
		case DEVICE4MB_IOCGETSET:
			temp = kmalloc(strlen((char *)arg)+1, GFP_KERNEL);
			for(i=0;i<strlen((char *)arg);i++)
				temp[i] = 0;
			if(copy_from_user(temp, (char *)arg, strlen((char *)arg))==0)
				tempret1 = 1;

			if(dev_msg)
			{
				if(copy_to_user((char *)arg, dev_msg, strlen(dev_msg))==0);
					tempret2 = 1;
				kfree(dev_msg);
			}
			dev_msg = temp;
			temp = NULL;
			if(tempret1 == 1 && tempret2 == 1)
				ret = 1;
			printk(KERN_WARNING "DEVICE4MB_IOCGETSET dev_msg is (%s)\n",dev_msg);
			break;
		default: return -ENOTTY;
	}
	return ret;	
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
	device4MB_data = kmalloc(sizeof(char)*DEVICE_MAX_SIZE, GFP_KERNEL);

	if (!device4MB_data) 
	{
		device4MB_exit();
	// cannot allocate memory
	// return no memory error, negative signify a failure
		return -ENOMEM;
	}
	// initialize the value 
	memset(device4MB_data,0,sizeof(char)*DEVICE_MAX_SIZE);
	startOffset = 0;
	endOffset = -1;
	printk(KERN_ALERT "This is a device4MB device module\n");
	return 0;
}

static void device4MB_exit(void)
{
	// if the pointer is pointing to something,  free the memory and assign the pointer to NULL
	if (device4MB_data) 
	{
		kfree(device4MB_data);
		device4MB_data = NULL;
	}
	if(dev_msg)
	{
		kfree(dev_msg);
		dev_msg = NULL;
	}
	startOffset = 0;
	endOffset = -1;

	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "device4MB");
}
MODULE_LICENSE("GPL");
module_init(device4MB_init);
module_exit(device4MB_exit);
