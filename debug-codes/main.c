#include "./include/ds18b20_driver.h"

#if 0
/*Debug*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#define IS_NEGATIVE(hex_value)		(hex_value & 0xF800)
#define BIT2VALUE(bit)	positive_bit2value_lookup_table[bit]
	
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

/*Debug*/
static void debug_bits(unsigned short hex)
{
	static int index = 0;
	static int counter = 0;
	char c = '0';
		
	if (index > 15){
		index = 0;
		printf("\n");
		return ;
	}
	

	
	if (hex & (1 << index)){
		c = '1';
	}
	
	index++;
	debug_bits(hex);


	
	printf("%c", c);
	counter++;
	
	if (counter % 4 == 0){
		printf(" ");
	} 
}

#if 0
int main(int argc, char *argv[])
{
	unsigned short dummy_reg_value;
	
	scanf("%x", &dummy_reg_value);
	
	debug_bits(dummy_reg_value);
	
	printf("Input value = 0x%x, temperature = %0.4f\n", dummy_reg_value, ds18b20_hex2temperature(dummy_reg_value));
	
	printf("Debug for macro DS18B20_COMMAND(READ_ROM) = 0x%x\n", DS18B20_COMMAND(READ_ROM));
	
	system("Pause");
	
	return 0;
}
#endif
