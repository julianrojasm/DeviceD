/*
	Authors: Brett Belcher & Julian Rojas
	Program: Device Driver
	Course: COP 4600 - Operating Systems
	Due Date: 4/25/2016
*/

//Includes and definitions derived from source material
#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <asm/uaccess.h>          // Required for the copy to user function
#define  DEVICE_NAME "projectdriver"    ///< The device will appear at /dev/projectdriver using this value
#define  CLASS_NAME  "project"        ///< The device class -- this is a character device driver
#define  BUFFER_SIZE 1025

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Brett and Julian");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Character Device Driver");  ///< The description -- see modinfo
MODULE_VERSION("0.1");            ///< A version number to inform users

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static char   message[BUFFER_SIZE] = { 0 };    ///< Memory for the string that is passed from userspace
static short  size_of_message;              ///< Used to remember the size of the string stored
static struct class*  projectdriverClass = NULL; ///< The device-driver class struct pointer
static struct device* projectdriverDevice = NULL; ///< The device-driver device struct pointer
//End of section derived from source material

//Prototypes
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

//File operations structure definition
static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

int init_module(void)
{
	printk(KERN_INFO "Beginning Initializing Kernal Module\n");

	//Dynamically allocate major number
	majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
	if (majorNumber < 0)
	{
		printk(KERN_ALERT "Failed To Register Valid Major Number\n");
		return majorNumber;
	}
	printk(KERN_INFO "Device Registered With Major Number %d\n", majorNumber);

	//Register class
	projectdriverClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(projectdriverClass))
	{
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed To Register Device Class\n");
		return PTR_ERR(projectdriverClass);
	}
	printk(KERN_INFO "Device Class Registered\n");

	projectdriverDevice = device_create(projectdriverClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if (IS_ERR(projectdriverDevice))
	{
		class_destroy(projectdriverClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed To Register Device\n");
		return PTR_ERR(projectdriverDevice);
	}
	printk(KERN_INFO "Device Fully Registered\n");

	return 0;
}

void cleanup_module(void)
{
	device_destroy(projectdriverClass, MKDEV(majorNumber, 0));
	class_unregister(projectdriverClass);
	class_destroy(projectdriverClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "Goodbye From Brett and Julian\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
	printk(KERN_INFO "Brett and Julian's Device Has Been Opened\n");
	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
	int errors = 0;
	errors = copy_to_user(buffer, message, size_of_message);

	if (errors == 0 && size_of_message != 0)
	{
		printk(KERN_INFO "Sent %d Characters To User\n", size_of_message);
		size_of_message = 0;
		memset(&message[0], 0, sizeof(message));
		return size_of_message;
	}
	else if (size_of_message == 0)
	{
		printk(KERN_INFO "No Characters To Be Sent To User\n");
		return size_of_message;
	}
	else 
	{
		printk(KERN_INFO "Failed To Send Characters To User\n");
		return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
	}
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
	sprintf(message, "%s", buffer);   // appending received string with its length
	size_of_message = strlen(message);                 // store the length of the stored message
	printk(KERN_INFO "Received %d characters from the user\n", len);
	return len;
}

static int dev_release(struct inode *inodep, struct file *filep) {
	printk(KERN_INFO "Brett and Julian's Device Has Now Closed\n");
	return 0;
}