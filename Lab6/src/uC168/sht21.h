//sht21.h

#define F_CPU 16000000UL
#define TRUE  1
#define FALSE 0

#include "twi_master.h"

#define SHT21_ADDRESS 0X80
#define SHT21_HOLD_TEMP 0xE3
#define SHT21_NOHOLD_TEMP 0xF3
#define SHT21_SOFT_RESET 0xFE

uint16_t read_temperature_SHT21();
void sht21_temp_convert(char temp_digits[], uint16_t sht21_temp, uint8_t f_not_c);
