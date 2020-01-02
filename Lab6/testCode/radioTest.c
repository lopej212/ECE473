 //Author: Jose Manuel Lopez Alcala
//ID:932503918
//Test code for radio

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>
//#include <util/twi.h>
#include <util/delay.h>
#include "twi_master.h"
#include "si4734.h"

//Bit Macros
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7

//set to KRKT Albany at 99.9Mhz initially
volatile uint16_t  current_fm_freq =  9990;
extern uint8_t  si4734_wr_buf[9];
extern uint8_t  si4734_rd_buf[9];
extern uint8_t  si4734_tune_status_buf[8];
extern volatile uint8_t STC_interrupt;  //flag bit to indicate tune or seek is done

//***********************************************************************
//                           init_tcnt3
//***********************************************************************
void init_tcnt3(){
  //Non-inverting mode //FAST PWM, 8-bit, no prescale
  TCCR3A |= (1<<COM3A1)|(0<<COM3A0)|(0<<WGM31)|(1<<WGM30);
  TCCR3B |= (0<<WGM33)|(1<<WGM32)|(1<<CS30);
  ETIMSK |= (1<<TOIE3);//overflow interrup
  OCR3A =  127;//Vout = OCR3A*(0.0198)+0.022
}

ISR(TIMER3_OVF_vect){
  //nothing for now
}

ISR(INT7_vect){
  STC_interrupt = TRUE;
  // if (STC_interrupt== TRUE) {
    // _delay_ms(1000);//for testing
    // PORTB ^= (1<<BIT4);//for testing
    // _delay_ms(1000);//for testing
    // PORTB ^= (1<<BIT4);//for testing
  // }

}

void radio_init_m(){
  PORTE &= ~(1<BIT7);//interupt 2 initially low to sense I2C mode
  DDRE  |= 0x80;///turn on port E bit 7 to drive it low
  PORTE |= (1<<BIT2);//hardware reset
  _delay_us(200);
  PORTE &= ~(1<<BIT2);//release Reset
  _delay_us(30);
  DDRE &= ~(0x80);
}

int main()
{
  DDRE |= (1<<BIT3);//for VOL control

  DDRE |= 0x04;//PORTE 2 output for radio reset
  DDRB  = 0x10;//set bit 4 output led for testing
  PORTE|= 0x04;//Reset radio on powerup
  EICRB |= (1<<BIT7) | (1<<BIT6);
  EIMSK|= (1<<BIT7);


  init_twi();
  init_tcnt3();
  radio_init_m();//initialize radio
  sei();

  //fm_pwr_up();
  fm_pwr_up();
  fm_tune_freq();//tune to frequency


  while (1)
  {
    _delay_ms(125);//for testing
    PORTB ^= (1<<BIT4);//for testing
    _delay_ms(500);//for testing
    PORTB ^= (1<<BIT4);//for testing
  }




  return 0;
}
