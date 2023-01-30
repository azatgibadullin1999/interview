#ifndef LINUX_KERNEL_LARLENA_MODULE_H_
# define LINUX_KERNEL_LARLENA_MODULE_H_

#include <linux/module.h>
#include <linux/kernel.h> // printk()
#include <linux/types.h> // dev_t
#include <linux/fs.h> // register_chardev_region() ...
#include <linux/moduleparam.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <asm/ioctl.h>
#include <linux/mutex.h>
#include <linux/wait.h>

#ifndef PIPE_MAJOR
# define PIPE_MAJOR 0
#endif

#ifndef PIPE_NR_DEVS
# define PIPE_NR_DEVS 4
#endif

#ifndef PIPE_BUFFER
# define PIPE_BUFFER 0x10000
#endif


#define FT_PIPE_IOC_MAGIC 'f'

#define FT_PIPE_IOCTSIZE	_IO(FT_PIPE_IOC_MAGIC, 0xA0)
#define FT_PIPE_IOCQSIZE	_IO(FT_PIPE_IOC_MAGIC, 0xA1)


#define FT_IOC_MAXNR	0xA1


#endif