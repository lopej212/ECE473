// testled1.c
// R. Traylor
// 10.3.05
// Tests wiring of LED board to mega128.
// Select a digit with PORTB upper nibble then with
// port D push buttons illuminate a single segment.

// Port mapping:
// Port A:  bit0 brown  segment A
//          bit1 red    segment B
//          bit2 orange segment C
//          bit3 yellow segment D
//          bit4 green  segment E
//          bit5 blue   segment F
//          bit6 purple segment G
//          bit7 grey   decimal point
//               black  Vdd
//               white  Vss

// Port C:  bit4 green  seg0
//          bit5 blue   seg1
//          bit6 purple seg2
//          bit7 grey   pwm
//               black  Vdd
//               white  Vss

#include <avr/io.h>
#include <util/delay.h>
int main()
{
DDRA  = 0xFF;   //set port A to all outputs
DDRC  = 0xF0;   //set port bits 4-7 B as outputs
DDRD  = 0x00;   //set port D all inputs
PORTD = 0xFF;   //set port D all pullups
PORTA = 0xFF;   //set port A to all ones  (off, active low)

while(1){
  //PORTC = 0x00; //digit zero  on
  //PORTC = 0x10; //digit one   on
  //PORTC = 0x20; //colon, indicator leds  on
  //PORTC = 0x30; //digit two   on
  PORTC = 0x40; //digit three on

//IF the following code is uncommnented,
//it will cycle through all of the segements
  PORTA = 0b11111110 ;
  _delay_ms(200);
  PORTA = 0b11111101 ;
  _delay_ms(200);
  PORTA = 0b11111011 ;
  _delay_ms(200);
  PORTA = 0b11110111 ;
  _delay_ms(200);
  PORTA = 0b11101111 ;
  _delay_ms(200);
  PORTA = 0b11011111 ;
  _delay_ms(200);
  PORTA = 0b10111111 ;
  _delay_ms(200);
  PORTA = 0b01111111 ;
  _delay_ms(200);
//***************************************

 //PORTA = 0b00000000;

  //PORTA = PIND;



 } //while
}  //main
