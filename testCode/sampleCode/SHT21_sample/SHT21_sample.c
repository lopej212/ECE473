
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "i2cmaster.h"
#include "uart.h"
#include "sht21.h"

int main(void)
{
	DDRC=(1<<PC0)|(1<<PC1);
	PORTC=(1<<PC0)|(1<<PC1);
	
	i2c_init();
	usart_inc();
	_delay_ms(100);
	usart_send_string("SHT21 Readings:\r\n");
	SHT21_reset();
	
    while(1)
    {
		SHT21_reset();
		write_user_register();
		serial_number();
		get_temperature();
		get_humidity();
		dew_point();
		
		usart_send_string("---------------------------------------------------\r\n");
		_delay_ms(1000);
    }
}