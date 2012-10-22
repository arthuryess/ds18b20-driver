#ifndef __PLATFORM_DEP_H__
#define __PLATFORM_DEP_H__

/*Platform dependent -- all functions should return 0 on success, -1 on failure*/
typedef struct platform_operations{
	int (*reset)(); /*Reset and presence procedure*/
	int (*write_byte)(unsigned char data);
	int (*read_byte)(unsigned char *data);
}platform_operation_t;

/*Supported platforms*/
#define PLATFORM_OK6410		0x0

/*Default platform is OK6410*/
#ifndef PLATFORM
#define PLATFORM			PLATFORM_OK6410
extern struct platform_operations ok6410_ds18b20_ps;
#define platform_ops		ok6410_ds18b20_ops
#endif


#endif /*PLATFORM_DEP_H*/
