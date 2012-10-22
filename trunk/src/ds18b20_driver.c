#include "ds18b20_driver.h"

#define EPSION								0.05f
#define ATTACH_FN(p_ds18b20, func_name)		do{(p_ds18b20)->func_name = ds18b20_##func_name;}while(0)				
#define IS_NEGATIVE(hex_value)				(hex_value & 0xF800)
#define BIT2VALUE(bit)						positive_bit2value_lookup_table[bit]


static float positive_bit2value_lookup_table[] = {
	0.0625f, /*Bit 0*/
	0.125f, /*Bit 1*/
	0.25f, /*Bit 2*/
	0.5f, /*Bit 3*/
	1.0f, /*Bit 4*/
	2.0f, /*Bit 5*/
	4.0f, /*Bit 6*/
	8.0f, /*Bit 7*/
	16.0f, /*Bit 8*/
	32.0f, /*Bit 9*/
	64.0f, /*Bit 10*/
};

static int ds18b20_set_resolution(ds18b20_t *ds18b20, int resolution)
{
	unsigned char  cfg;
	ds18b20_hal_t *hal = ds18b20->hal;
	
	switch (resolution){
		case 9:
			cfg = DS18B20_CFG_RESOLUTION(9);
			hal->conv_time = DS18B20_CONV_TIME(9);
			break;
		case 10:
			cfg = DS18B20_CFG_RESOLUTION(10);
			hal->conv_time = DS18B20_CONV_TIME(10);
			break;
		case 11:
			cfg = DS18B20_CFG_RESOLUTION(11);
			hal->conv_time = DS18B20_CONV_TIME(11);
			break;
		case 12:
			cfg = DS18B20_CFG_RESOLUTION(12);
			hal->conv_time = DS18B20_CONV_TIME(12);
			break;
		default:
			return -1;
	}
	
	hal->resolution = resolution;
	
	return ds18b20->hal->write_scratchpad(hal, (unsigned char *)&ds18b20->mem_data.th);
}

static int ds18b20_read_rom(ds18b20_t *ds18b20)
{
	if (devices_on_bus <= 0){
		return -1;
	}
	
	ds18b20->rom_data = devices_roms[ds18b20->hal->device_id];
	
	return 0;
}

static int ds18b20_write_alarm(ds18b20_t *ds18b20, float temperature)
{	
	unsigned short th_tl = ds18b20_temperature2hex(temperature);
	
	ds18b20->mem_data.th = (unsigned char)((th_tl >> 8) & 0xFF);
	ds18b20->mem_data.tl = (unsigned char)(th_tl & 0xFF);
	
	return ds18b20->hal->write_scratchpad(ds18b20->hal, (unsigned char *)&ds18b20->mem_data.th);
}

static int ds18b20_read_memory(ds18b20_t *ds18b20, int n_bytes)
{
	return ds18b20->hal->read_scratchpad(ds18b20->hal, (unsigned char *)&ds18b20->mem_data, n_bytes);
}

/*Initialize ds18b20*/
int ds18b20_init(ds18b20_t *ds18b20, ds18b20_hal_t *hal)
{
	if (ds18b20 == NULL || hal == NULL){
		return -1;
	}
	
	memset(ds18b20, 0, sizeof(ds18b20_t));
	ds18b20->hal = hal;

	ATTACH_FN(ds18b20, set_resolution);
	ATTACH_FN(ds18b20, read_rom);
	ATTACH_FN(ds18b20, write_alarm);
	ATTACH_FN(ds18b20, read_memory);
	
	if (ds18b20_hal_init(hal))	
	
	/*Fill mem_data with scratchpad*/
	return ds18b20->read_memory(ds18b20, DS18B20_SCRATCHPAD_SIZE);
}

/*ds18b20_hex2temperature -- Caculate temperature according to hex_value*/
float ds18b20_hex2temperature(unsigned short hex_value)
{
	int bit_index = 0;
	float temperature = 0.0f;
	
	if (IS_NEGATIVE(hex_value)){
		
		hex_value--;
		
		while (bit_index <= DS18B20_TEMPERATURE_DATA_MAX_BIT){
			if ((hex_value & (1 << bit_index)) == 0){
				temperature += BIT2VALUE(bit_index);
			}
			
			bit_index++;	
		}
		
		temperature = -temperature;
	}else{
		
		while (bit_index <= DS18B20_TEMPERATURE_DATA_MAX_BIT){
			if (hex_value & (1 << bit_index)){
				temperature += BIT2VALUE(bit_index);
			}
			
			bit_index++;
		}
	}
	
	return temperature;
}

unsigned short ds18b20_temperature2hex(float temperature)
{
	/*
		Minimum resolution unit is 0.0625f
		Maximum temperature that DS18B20 can measure is 125.0 and minimum is -55.0
	*/
	unsigned short ret = 0;
	int n;
	int is_negative = 0;
	int intPart = (int)temperature;
	float decimalPart = fabs(temperature - intPart);
	
	if (temperature <= EPSION){
		is_negative = 1;
	}
	
	if (intPart > DS18B20_MAX_TEMPERATURE){
		ret = 0x7D0;
		goto out;
	}
	
	if (intPart < DS18B20_MIN_TEMPERATURE){
		ret = 0xFC90;
		goto out;
	}
	
	if (temperature >= -EPSION && temperature <= EPSION){
		ret = 0x0;
		goto out;
	}
	
	if (is_negative){
		intPart = -intPart;
	}
	
	ret |= (intPart << 4);
	
	n = decimalPart / DS18B20_MEASURE_UNIT;
	
	ret |= (unsigned short)n;
	
	if (is_negative){
		/*High 5 bits are 1*/
		ret = -ret;
		ret |= 0xF800;
	}

out:
	return ret;
}



#if 0
/*main() is only used for debuggine purpose*/
int main()
{
	float temperature;
	
	printf("Enter temperature:\n");
	scanf("%f", &temperature);
	printf("Hex value for %f is 0x%x\n", temperature, ds18b20_temperature2hex(temperature));
	printf("Hex value 0x%x to float is %f\n", ds18b20_temperature2hex(temperature), ds18b20_hex2temperature(ds18b20_temperature2hex(temperature)));
	
	system("Pause");
	return 0;
}
#endif
