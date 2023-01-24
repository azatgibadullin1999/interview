#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");


static int __init ft_larlenas_driver_init(void) {
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}

static void __exit ft_larlenas_driver_exit(void) {
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
}

module_init(ft_larlenas_driver_init);
module_exit(ft_larlenas_driver_exit);

