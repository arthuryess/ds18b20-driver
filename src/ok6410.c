#include "platform_dep.h"
#include "ds18b20_hal.h"

platform_operation_t ok6410_ds18b20_ops = {
	ok6410_ds18b20_reset,
	ok6410_ds18b20_write_byte,
	ok6410_ds18b20_read_byte,
};
