#include "ds18b20_driver.h"

#define MODULE_NAME		"Linux DS18B20 driver"
/*Global*/
typedef struct ds18b20_dev{
	int ndevs;
	ds18b20_t *devs;
	/*TODO: add character device's structures and lock structures*/
}ds18b20_dev_t;

ds18b20_dev_t ds18b20_devs;

/*Linux kernel module*/
static int __init linux_ds18b20_init(void)
{
	int ret;
	
	printk("Initializing DS18B20 for %s...\n", PLATFROM_NAME);
	
	ret = ds18b20_hal_probe();
	
	if (ret != 0 || devices_on_bus == 0){
		printk("No DS18B20 found on 1-wire bus on %s\n", PLATFORM_NAME);
		return -ENODEV;
	}

	ds18b20_devs.devs = kmalloc(devices_on_bus * sizeof(ds18b20_dev_t));
	
	if (ds18b20_devs.devs == NULL){
		printk("Cannot allocate enough memory for this module\n");
		return -ENOMEM;
	}
	
	ds18b20_devs.ndevs = devices_on_bus;
	
	{
		float temperature;
		/*Debug codes here*/
		ds18b20_devs.devs[0].read_temperature(ds18b20_devs.devs, &temperature);
		
		printk(KERN_EMERG "%s: Temperature is %0.3f Centigrade\n", MODULE_NAME);
	}
	
	printk("DS18B20 driver initialized\n");
	
	/*In debug mode, return failure*/
	return -ENODEV;
}

static void __exit linux_ds18b20_exit(void)
{
	
}

MODULE_LICENSE("GPL v2");
module_init(linux_ds18b20_init);
module_exit(linux_ds18b20_exit);
