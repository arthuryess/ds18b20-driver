A general ds18b20 device driver, three layers:
1. General driver layer
> All logical stuffs
2. HAL layer
> Hardware abstract layer -- how to access DS18B20
3. Platform (OS and platform specific)
> Generate I/O singals