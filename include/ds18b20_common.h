#ifndef __DS18B20_COMMON_H__
#define __DS18B20_COMMON_H__

#define DS18B20_MAX_TEMPERATURE			125
#define DS18B20_MIN_TEMPERATURE			-55
#define DS18B20_MEASURE_UNIT			0.0625f

#define DS18B20_SCRATCHPAD_SIZE			0x8
/*	Command
	Example: DS18B20_COMMMAND(READ_ROM) returns 0x33
*/
#define DS18B20_COMMAND(command)			DS18B20_COMMAND_##command
/*ds18b20 rom command*/
#define DS18B20_COMMAND_READ_ROM			(0x33)
#define DS18B20_COMMAND_MATCH_ROM			(0x55)
#define DS18B20_COMMAND_SKIP_ROM			(0xCC)
#define DS18B20_COMMAND_SEARCH_ROM			(0xF0)
#define DS18B20_COMMAND_ALARM_SEARCH		(0xEC)
/*ds18b20 memory command*/
#define DS18B20_COMMAND_WRITE_SCRATCHPAD	(0x4E)
#define DS18B20_COMMAND_READ_SCRATCHPAD		(0xBE)
#define DS18B20_COMMAND_COPY_SCRATCHPAD		(0x48)
#define DS18B20_COMMAND_CONVERT_T			(0x44)
#define DS18B20_COMMAND_RECALL_E2			(0xB8)
#define DS18B20_COMMAND_READ_POWER_SUPPLY	(0xB4)

/*Resolution*/
#define DS18B20_CFG_RESERVED				0x1F
#define DS18B20_CFG_RESOLUTION(resolution)	DS18B20_CFG_RESOLUTION_##resolution
#define DS18B20_CFG_RESOLUTION_9			(DS18B20_CFG_RESERVED)
#define DS18B20_CFG_RESOLUTION_10			(DS18B20_CFG_RESERVED | (0x1 << 5))
#define DS18B20_CFG_RESOLUTION_11			(DS18B20_CFG_RESERVED | (0x2 << 5))
#define DS18B20_CFG_RESOLUTION_12			(DS18B20_CFG_RESERVED | (0x3 << 5))

#define DS18B20_CONV_TIME(resolution)		DS18B20_CONV_TIME_##resolution
#define DS18B20_CONV_TIME_9					93.75f
#define DS18B20_CONV_TIME_10				187.5f
#define DS18B20_CONV_TIME_11				375.0f
#define DS18B20_CONV_TIME_12				750.0F	

#define DS18B20_POWER_SUPPLY_PARASITE		0x0		/*Not supported for now*/
#define DS18B20_POWER_SUPPLY_EXTERNAL		0x1


struct ds18b20_hal_rom{
	unsigned char crc;
	unsigned char serial_number[6];
	unsigned char family_code;
};

struct ds18b20_hal_memory{
	unsigned char temperature_lsb;
	unsigned char temperature_msb;
	unsigned char th;
	unsigned char tl;
	unsigned char config;
	unsigned char reserved[3];
	unsigned char crc;
};


#endif /*DS18B20_COMMON_H*/
