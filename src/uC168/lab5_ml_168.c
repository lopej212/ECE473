//Author: Jose Manuel Lopez Alcala
//ID:932503918
//ECE 473 Lab5&6
//Fall 2019
//Code for ATmega168

#include <avr/io.h>
#include "uart_functions_m168.h"
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include "sht21.h"
#include <stdlib.h>

//Bit Macros
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7

volatile uint8_t rcv_rdy=0;
char rx_char;
//Array to hold incoming data
char rx_array[16];
//counter for decode_rx_data()
uint8_t i_decode;
//Variable to hold length of string in decode_rx_data()
uint8_t length;
//Flag to take sample
uint8_t sample_flag=0;
//Compare array
char sample_array[] = "sample";
//Transmission array
char tx_array[16];

ISR(USART_RX_vect){
  static uint8_t i;
  rx_char = UDR0;//get character
  rx_array[i++]=rx_char;//store in array
  if (rx_char=='\0') {
    rcv_rdy ^=(1<<BIT0);;
    i=0;//reset counter
  }
}


int main()
{
  uart_init();
  sei();
  DDRB = 0x20;// Setting LED Pin as output
  //SHT21 Temperature value
  uint16_t SHT21_temp_val;
  uint8_t fc_toggle = 0;

  while (1) {
    SHT21_temp_val = read_temperature_SHT21();//read temperature sensor
    _delay_ms(500);//for testing
    PORTB ^= (1<<5);//for testing
    _delay_ms(500);//for testing
    PORTB ^= (1<<5);//for testings

    if (rcv_rdy==1) {
      rcv_rdy ^= (1<<BIT0);
      fc_toggle ^= (1<<BIT0);
      sht21_temp_convert(tx_array,SHT21_temp_val,fc_toggle);//convert data
      //itoa(1024,tx_array,10);
      uart_puts(tx_array);//send data back
      uart_putc('\0');
    }
  }

  return 0;
}
