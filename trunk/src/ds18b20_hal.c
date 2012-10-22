#include "ds18b20_hal.h"

#define ATTACH_HAL_FN(p_hal, func_name)		do{(p_hal)->func_name = ds18b20_hal_##func_name;}while(0)

int devices_on_bus;
struct ds18b20_hal_rom *devices_roms;
ds18b20_hal_t *devices_detected;

static inline void ds18b20_hal_match_rom(int device_id)
{
	if (devices_on_bus == 1){
		platform_ops.write_byte(DS18B20_COMMAND(SKIP_ROM));
	}else if (devices_on_bus > 1){
		/*Send MATCH_ROM command*/
		int i;
		unsigned char *rom_code = (unsigned char *)&devices_roms[device_id];
		
		platform_ops.write_byte(DS18B20_COMMAND(MATCH_ROM));
		
		for (i = 0; i < 8; i++){
			platform_ops.write_byte(rom_code[i]);
		}
	}else{
		return -1;
	}
	
	return 0;
}

static int ds18b20_hal_write_scratchpad(ds18b20_hal_t *hal, unsigned char *data)
{
	int i;
	
	platform_ops.reset();
	
	if (ds18b20_hal_match_rom(hal->device_id) != 0){
		return -1;
	}
	
	platform_ops.write_byte(DS18B20_COMMAND(WRITE_SCRATCHPAD));
	
	for (i = 0; i < 3; i++){
		platform_ops.write_byte(*(data + i));
	}
	
	return 0;
}

static int ds18b20_hal_read_scratchpad(ds18b20_hal_t *hal, unsigned char *data, int len)
{
	int i;
	
	if (len < 0 || len > DS18B20_SCRATCHPAD_SIZE){
		len = 8;
	}
	
	platform_ops.reset();
	
	if (ds18b20_hal_match_rom(hal->device_id) != 0){
		return -1;
	}
	
	platform_ops.write_byte(DS18B20_COMMAND(READ_SCRATCHPAD));
	
	for (i = 0; i < len; i++){
		platform_ops.read_byte(data + i);
	}
	
	
	return 0;
}

#define DS18B20_HAL_COPY_SCRATCHPAD_MAX_RETRIES			20

static int ds18b20_hal_copy_scratchpad(ds18b20_hal_t *hal)
{
	unsigned char busy;
	int retry_count = 0;
	
	platform_ops.reset();
	
	if (ds18b20_hal_match_rom(hal->device_id) != 0){
		return -1;
	}
	
	platform_ops.write_byte(DS18B20_COMMAND(COPY_SCRATCHPAD));
	
	if (hal->power_supply_mode == DS18B20_POWER_SUPPLY_PARASITE){
		/*Not supported now*/
		return -1;
	}
	
	do {
		platform_ops.read_byte(&busy);
		MDELAY(3);
		retry_count++;
	}while((busy == 0) && retry_count < DS18B20_HAL_COPY_SCRATCHPAD_MAX_RETRIES);
	
	if ((busy == 0) && retry_count == DS18B20_HAL_COPY_SCRATCHPAD_MAX_RETRIES){
		return -1;
	}
	
	return 0;
}

#define DS18B20_HAL_CONV_MAX_RETRIES		3

static int ds18b20_hal_convert_temperature(ds18b20_hal_t *hal)
{
	unsigned char busy;
	int retry_count = 0;
	
	platform_ops.reset();
	
	if (ds18b20_hal_match_rom(hal->device_id) != 0){
		return -1;
	}
	
	platform_ops.write_byte(DS18B20_COMMAND(CONVERT_T));
	
	if (hal->power_supply_mode == DS18B20_POWER_SUPPLY_PARASITE){
		/*Not supported*/
		return -1;
	}
	
	do {
		platform_ops.read_byte(&busy);
		MDELAY((int)hal->conv_time);
		retry_count++;
	}while((busy == 0) && retry_count < DS18B20_HAL_CONV_MAX_RETRIES);
	
	if ((busy == 0) && retry_count == DS18B20_HAL_CONV_MAX_RETRIES){
		return -1;
	}
	
	return 0;
}

/*Determine how many devices on the bus, devices_on_bus and devices_roms will be set*/
int ds18b20_hal_probe()
{
	/*Currently only support 1 device on bus*/
	int i;
	unsigned char ret;
	
	devices_on_bus = 1;
	OS_MALLOC(devices_roms, sizeof(struct ds18b20_hal_rom) * devices_on_bus);
	OS_MALLOC(devices_detected, sizeof(ds18b20_hal_t) * devices_on_bus);
	
	if (devices_roms == NULL || devices_detected == NULL){
		goto err;
	}
	
	/*Setup devices_roms*/
	platform_ops.reset();
	platform_ops.write_byte(DS18B20_COMMAND(READ_ROM));
	/*Read family code*/
	platform_ops.read_byte(&devices_roms[0].family_code);
	/*Six bytes serial numbers*/
	for (i = 0; i < 6; i++){
		platform_ops.read_byte(&devices_roms[0].serial_number[i]);
	}
	/*CRC*/
	platform_ops.read_byte(&device_roms[0].crc);

	return 0;
err:
	if (devices_roms != NULL){
		OS_FREE(devices_roms);
		devices_roms = NULL;
	}
	
	if (devices_detected != NULL){
		OS_FREE(devices_detected);
		devices_detected = NULL;
	}
	
	devices_on_bus = 0;
	
	return -1;
}

int ds18b20_hal_init(ds18b20_hal_t *hal)
{
	int i;
	unsigned char ret;
	
	if (hal == NULL){
		return -1;
	}
	
	/*Reset and Presense*/
	if (platfrom_ops.reset() != 0){
		goto err;
	}
	
	/*Get power supply mode*/
	platform_ops.reset();
	
	if (ds18b20_hal_match_rom(hal->device_id) != 0){
		goto err;
	}
	
	platform_ops.write_byte(DS18B20_COMMAND(READ_POWER_SUPPLY));
	platform_ops.read_byte(&ret);
	
	hal->power_supply_mode = (ret) ? DS18B20_POWER_SUPPLY_EXTERNAL : DS18B20_POWER_SUPPLY_PARASITE;
	
	/*Get resolution*/
	platform_ops.reset();	

	if (ds18b20_hal_match_rom(hal->device_id) != 0){
		goto err;
	}
	
	platform_ops.write_byte(DS18B20_COMMAND(READ_SCRATCHPAD));
	/*Skip first 4 bytes*/
	for (i = 0; i < 4; i++){
		platform_ops.read_byte(&ret);
	}
	platform_ops.read_byte(&ret);
	
	switch (ret){
		case DS18B20_CFG_RESOLUTION(9):
			hal->resolution = 9;
			hal->conv_time = DS18B20_CONV_TIME(9);			
			break;
		case DS18B20_CFG_RESOLUTION(10):
			hal->resolution = 10;
			hal->conv_time = DS18B20_CONV_TIME(10);
			break;
		case DS18B20_CFG_RESOLUTION(11):
			hal->resolution = 11;
			hal->conv_time = DS18B20_CONV_TIME(11);
			break;
		case DS18B20_CFG_RESOLUTION(12):
		default:
			hal->resolution = 12;
			hal->conv_time = DS18B20_CONV_TIME(12);
			break;
	}
	
	ATTACH_HAL_FN(hal, write_scratchpad);
	ATTACH_HAL_FN(hal, read_scratchpad);
	ATTACH_HAL_FN(hal, copy_scratchpad);
	ATTACH_HAL_FN(hal, convert_temperature);

	return 0;
err:
	return -1;
}

void ds18b20_hal_remove()
{
	if (devices_roms != NULL){
		OS_FREE(devices_roms);
		devices_roms = NULL;
	}
	
	if (devices_detected != NULL){
		OS_FREE(devices_detected);
		devices_detected = NULL;
	}
	
	devices_on_bus = 0;
}
