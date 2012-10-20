#ifndef __DS18B20_DRIVER_H__
#define __DS18B20_DRIVER_H__

#include "ds18b20_hal.h"
/*Linux kernel headers*/
#ifdef __KERNEL__
#include <linux/init.h>
#include <linux/types.h>
#else
/*Debug*/
#include <stdio.h>
#endif

#define DS18B20_TEMPERATURE_DATA_MAX_BIT	10

typedef struct ds18b20{
	struct ds18b20_hal_rom rom_data;
	struct ds18b20_hal_memory mem_data;
	/*HAL layer*/
	ds18b20_hal_t *hal;
	
	/*General*/
	int (*set_resolution)(struct ds18b20 *ds18b20, int resolution); /*Resolution can be 9 to 12, other value is invalid*/
	int (*read_rom)(struct ds18b20 *ds18b20);
	int (*read_temperature)(struct ds18b20 *ds18b20, float *temperature); /*Do a convert and get temperature*/
	int (*write_alarm)(struct ds18b20 *ds18b20, float temperature);
	int (*save_cfg)(struct ds18b20 *ds18b20);
	int (*read_memory)(struct ds18b20 *ds18b20, int n_bytes); /*If n_bytes is less than 0 or greater than 8, then all members in ds18b20_memory are read else only read n_bytes(1 - 8) and quit*/
}ds18b20_t;

/*Init DS18B20*/
int ds18b20_init(ds18b20_t *ds18b20, ds18b20_hal_t *hal);
/*Change hex value read from register to float */
float ds18b20_hex2temperature(unsigned short hex_value);
/*Change float value to hex value which is about to be written to TH/TL*/
unsigned short ds18b20_temperature2hex(float temperature);		

#endif /*DS18B20_DRIVER_H*/
