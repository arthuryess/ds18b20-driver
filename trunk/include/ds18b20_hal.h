#ifndef __DS18B20_HAL_H__
#define __DS18B20_HAL_H__

#include "os_dep.h"
#include "ds18b20_common.h"

typedef struct ds18b20_hal{
	int device_id;
	int power_supply_mode; /*External or parasite, default external*/
	int resolution;	/*Can be 9, 10, 11, 12, default 12*/
	float conv_time;	/*Default 750ms*/
	
	/*For upper layer use*/
	int (*write_scratchpad)(struct ds18b20_hal *hal, unsigned char *data);
	int (*read_scratchpad)(struct ds18b20_hal *hal, unsigned char *data, int len);
	int (*copy_scratchpad)(struct ds18b20_hal *hal);
	int (*convert_temperature)(struct ds18b20_hal *hal);
}ds18b20_hal_t;

/*Default HAL structure*/
extern ds18b20_hal_t ds18b20_default;
/*ROM codes for every device detected*/
extern int devices_on_bus;
extern struct ds18b20_hal_rom *devices_roms;
extern ds18b20_hal_t *devices_detected;

/*Determine how many devices on the bus, devices_on_bus and devices_roms will be set*/
int ds18b20_hal_probe();
/*Init one ds18b20_hal*/
int ds18b20_hal_init(ds18b20_hal_t *hal);


#endif /*DS18B20_HAL_H*/
