/*
 * sht21.c
 *
 * Created: 04-Nov-15 2:44:40 PM
 *  Author: Vrajlata
 */ 
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include "i2cmaster.h"
#include "sht21.h"
float rh,tc;

void SHT21_reset()
{
	unsigned char reg[1];
	reg[0]=SHT21_reset_cmd;
	i2c_run();
	i2c_write(SHT21_i2c_write);
	i2c_write(*reg);
	i2c_stop();
	_delay_ms(100);
}

uint16_t checksum(unsigned char data[],uint8_t byte, uint8_t check)
{
	uint8_t crc=0;
	uint8_t bytectr,bit;
	for (bytectr=0; bytectr<byte;bytectr++)
	{
		crc^=(data[bytectr]);
		for (bit=8;bit>0;bit--)
		{
			if(crc&0x80)
			{
				crc=(crc<<1)^polynomial;
			}
			else
			{
				crc=crc<<1;
			}
		}
	}
	if (crc!=check)
	{
		return 0;
	}
	else
	{
		return data;
	}
}

uint8_t read_user_register()
{
	uint8_t crc;
	unsigned char reg[2];
	reg[0]=user_register_read;
	//Request the user register
	i2c_run();
	i2c_write(SHT21_i2c_write);
	i2c_write(*reg);
	
	//Read result
	i2c_run();
	i2c_write(SHT21_i2c_read);
	reg[1] = i2c_readAck();
	crc=i2c_readNak();
	checksum(reg,2,crc);
	i2c_stop();
	return reg;
}

void write_user_register()
{
	uint8_t reg_value=0;
	unsigned char reg[3];
	reg[0]=user_register_write;
	reg[1]=0x44;
	
	i2c_run();
	i2c_write(SHT21_i2c_write);
	i2c_write(*reg);
	reg_value=reg[0];
	i2c_stop();
}

uint16_t read_value(uint8_t reg)
{
	char data[4],crc;
	uint16_t result;
	data[0]=reg;
	
	i2c_run();
	i2c_write(SHT21_i2c_write);
	i2c_write(*data);
	
	i2c_run();
	i2c_write(SHT21_i2c_read);
	data[1] = i2c_readAck();
	data[2] = i2c_readAck();
	crc=i2c_readNak();
	i2c_stop();
	
	result=(data[1]<<8) | data[2];
	checksum(result,4,crc);
	result &= 0xFFFC;
	return result;
}

void get_humidity()
{
	char buffer2[4];
	uint16_t hum_value=read_value(humidity_hold_mode);
	rh = -6 + 125.0 / 65536.0 * hum_value;
	dtostrf(rh,5,2,buffer2);
	usart_send_string("Humidity:");
	usart_send_string(buffer2);
	usart_send_string(" %RH\r\n");
	_delay_ms(100);
}

void get_temperature()
{
	char buffer1[4];
	uint16_t temp_value=read_value(temperature_hold_mode);
	tc = -46.85 + 175.72 / 65536.0 * temp_value;
	dtostrf(tc,5,2,buffer1);
	usart_send_string("Temperature:");
	usart_send_string(buffer1);
	usart_send_string(" °C\r\n");
	_delay_ms(100);
}

void dew_point()
{
	char buffer[4];
	float logEx, d_point;
	logEx = 0.66077 + 7.5 * tc / (237.3 + tc) + (log10(rh) - 2);
	d_point = (logEx - 0.66077) * 237.3 / (0.66077 + 7.5 - logEx);
	dtostrf(d_point,5,2,buffer);
	usart_send_string("Dew Point:");
	usart_send_string(buffer);
	usart_send_string("\r\n");
}

void serial_number()
{
	unsigned char serial[8],buffer[8];
	uint64_t crc,result;
	
	i2c_run();
	i2c_write(SHT21_i2c_write);
	i2c_write(0xFA);
	i2c_write(0x0F);
	
	i2c_run();
	i2c_write(SHT21_i2c_read);
	serial[5]=i2c_readAck();	//Read SNB_3
	i2c_readAck();
	serial[4]=i2c_readAck();	//Read SNB_2
	i2c_readAck();
	serial[3]=i2c_readAck();	//Read SNB_1
	i2c_readAck();
	serial[2]=i2c_readAck();	//Read SNB_0
	crc=i2c_readNak();
	i2c_stop();
	
	i2c_run();
	i2c_write(SHT21_i2c_write);
	i2c_write(0xFC);
	i2c_write(0xC9);
	
	i2c_run();
	i2c_write(SHT21_i2c_read);
	serial[1]=i2c_readAck();	//Read SNC_1
	serial[0]=i2c_readAck();	//Read SNC_0
	i2c_readAck();
	serial[7]=i2c_readAck();	//Read SNA_1
	serial[6]=i2c_readNak();	//Read SNA_0
	crc=i2c_readNak();
	i2c_stop();
	
	result=serial[7]<<56 | serial[6]<<48 | serial[5]<<40 | serial[4]<<32 | serial[3]<<24 | serial[2]<<16 | serial[1]<<8 | serial[0];
	//return serial;
	dtostrf(result,8,0,buffer);
	usart_send_string("Serial Number:");
	usart_send_string(buffer);
	usart_send_string("\r\n");
}
