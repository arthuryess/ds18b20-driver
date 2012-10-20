#ifndef __OS_DEP_H__
#define __OS_DEP_H__

#ifdef __LINUX__
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <asm/uaccess.h>
#include "linux_platform_dep.h"

#define UDELAY(us)		do{udelay(us);}while(0)
#define MDELAY(ms)		do{mdelay(ms);}while(0)
#define OS_MALLOC(ptr, size)	do{(ptr) = kmalloc((size));}while(0)		
#define OS_FREE(ptr)			do{kfree((ptr));}while(0)
#endif

#endif /*OS_DEP_H*/
