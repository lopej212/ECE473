#include <avr/io.h>
#include <util/delay.h>

int8_t debounce_switch() {
  static uint16_t state = 0; //holds present state
  state = (state << 1) | (! bit_is_clear(PINA, 7)) | 0xE000;
  if (state == 0xF000) return 1;
  return 0;
}

int main()
{
  DDRA = 0x00; // set port A to all inputs
  PORTA = 0xFF;// set port A all pullups
  DDRB = 0xFF; // set port bits  B as outputs
  PORTB = 0x00;
  DDRC = 0x00; //set PORT C as output
  PORTC = 0xF0;//power off to Seven segment and activate TRI Buffer

  while (1) {
    if (debounce_switch() ) {
      PORTB = 0x01;
      _delay_ms(500);
      PORTB = 0x00;
    }
    _delay_ms(2);
  }
}
