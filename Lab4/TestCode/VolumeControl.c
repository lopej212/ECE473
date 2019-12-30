#include <avr/io.h>
#include <avr/interrupt.h>

void init_tcnt3(){
  //Non-inverting mode //FAST PWM, 8-bit, no prescale
  TCCR3A |= (1<<COM3A1)|(0<<COM3A0)|(0<<WGM31)|(1<<WGM30);
  TCCR3B |= (0<<WGM33)|(1<<WGM32)|(1<<CS30);
  OCR3A =  150;
}

int main(){
  DDRE = (1<<PB3);
  init_tcnt3();
  while (1) {}
}
