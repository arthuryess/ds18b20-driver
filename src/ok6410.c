#include "platform_dep.h"
#include "ds18b20_hal.h"
#include <plat/gpio-cfg.h>
#include <mach/reg-gpio.h>

#define DQ_OUTPUT_MODE()		do{s3c_gpio_cfgpin(S3C64XX_GPE(0), S3C_GPIO_SFN(1));}while(0)
#define DQ_HIGH()				do{gpio_set_value(S3C64XX_GPE(0), 1);}while(0)
#define DQ_LOW()				do{gpio_set_value(S3C64XX_GPE(0), 0);}while(0)
#define DQ_INPUT_MODE()			do{s3c_gpio_cfgpin(S3C64XX_GPE(0), S3C_GPIO_SFN(0));}while(0)
#define DQ_STATUS()				gpio_get_value(S3C64XX_GPE(0))

int ok6410_ds18b20_reset()
{
	int ret;
	
	/*GPE0 as output*/
	DQ_OUPUT_MODE();
	DQ_HIGH();
	udelay(100);
	/*Apply reset pulse*/
	DQ_LOW();
	udelay(DS18B20_TIMING(RESET_PULSE_MIN) + 100);
	DQ_HIGH();
	/*Wait for ds18b20*/
	udelay(DS18B20_TIMING(WAIT_MAX) * 2);
	DQ_INPUT_MODE();
	/*Read pin*/
	ret = DQ_STATUS();
	/*Make sure DQ is high*/
	udelay(480);
	
	return ret;
}

int ok6410_ds18b20_write_byte(unsigned char data)
{
	int i;
	
	DQ_OUTPUT_MODE();
	
	for (i = 0; i < 8; i++){
		DQ_LOW();
		udelay(2);
		
		if (data & 0x1){
			DQ_HIGH();
		}
		
		/*Wait for DS18B20 to sample (+ 20us to make sure bit is sampled)*/
		udelay(DS18B20_TIMING(SAMPLE_MAX) + 20);
		DQ_HIGH();
		udelay(5);
		data >>= 1;
	}
	
	/*Make sure DQ is high*/
	DQ_HIGH();
	
	return 0; 
}

int ok6410_ds18b20_read_byte(unsigned char *data)
{
	int ret;
	int i;
	
	
	for (i = 0;i < 8; i++){
		DQ_OUTPUT_MODE();
		DQ_LOW();
		udelay(2);
		DQ_HIGH();
		/*Sample*/
		DQ_INPUT_MODE();
		udelay(2);
		
		ret = DQ_STATUS();
		
		if (ret != 0){
			*data |= (1 << i);
		}
		
		udelay(60);
	}
	
	return 0;
}

platform_operation_t ok6410_ds18b20_ops = {
	ok6410_ds18b20_reset,
	ok6410_ds18b20_write_byte,
	ok6410_ds18b20_read_byte,
};
