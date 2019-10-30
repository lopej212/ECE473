// lab2_ml2.c
// Manuel Lopez Alcala

//  HARDWARE SETUP:
//  PORTA is connected to the segments of the LED display. and to the pushbuttons.
//  PORTA.0 corresponds to segment a, PORTA.1 corresponds to segement b, etc.
//  PORTB bits 4-6 go to a,b,c inputs of the 74HC138.
//  PORTB bit 7 goes to the PWM transistor base.

#include <avr/io.h>
#include <util/delay.h>

//holds data to be sent to the segments. logic zero turns segment on
uint8_t segment_data[5];

//******************************************************************************
//                            debounce_switch
//Checks the state of the button number passed to it. It shifts in ones till
//the button is pushed. Function returns a 1 only once per debounced button
//push so a debounce and toggle function can be implemented at the same time.
//Adapted to check all buttons from Ganssel's "Guide to Debouncing"
//Expects active low pushbuttons on PINA port.  Debounce time is determined by
//external loop delay times 12.
//
uint8_t debounce_switch(uint8_t pin) {
  static uint16_t state[8] = {0};
  state[pin] = (state[pin]<<1)|(!bit_is_clear(PINA,pin))|0xE000;
  if (state[pin]==0xF000) {
    return 1;
  }
  return 0;
}

/***************Function: decoder()***************************
Description:
  Takes in an 8 bit decimal and returns the binary equivalent
  that will be used to display on the seven segement.
  Format:(DP)GFEDCBA
Parameters:
  - uint8_t decimal: This is the decimal that needs to be converted
Precondition: NONE
Return: uint_t binary equivalent
************************************************************************/
uint8_t decoder(uint8_t decimal)
{
  if (decimal == 0) {//decimal: 0
    return 0b11000000;
  }else if (decimal == 1) {//decimal: 1
    return 0b11111001;
  }else if (decimal == 2) {//decimal: 2
    return 0b10100100;
  }else if (decimal == 3) {//decimal: 3
    return 0b10110000;
  }else if (decimal == 4) {//deciaml: 4
    return 0b10011001;
  }else if (decimal == 5) {//decimal: 5
    return 0b10010010;
  }else if (decimal == 6) {//deciaml: 6
    return 0b10000010;
  }else if (decimal == 7) {//decimal: 7
    return 0b11111000;
  }else if (decimal == 8) {//decimal: 8
    return 0b10000000;
  }else if (decimal == 9) {//decimal: 9
    return 0b10010000;
  }
  //should never reach this return
  return 0b11111111;// All segments OFF
}

//***********************************************************************************
//                                   segment_sum
//takes a 16-bit binary input value and places the appropriate equivalent 4 digit
//BCD segment code in the array segment_data for display.
//array is loaded at exit as:  |digit0|digit1|colon(digit2)|digit3|digit4|
void segsum(uint16_t sum) {
  uint8_t thousands = (sum/1000)%10;//parses the thousands places
  uint8_t hundreds  = (sum/100)%10;//parses the hundreds place
  uint8_t tens      = (sum/10)%10;//parses the tens places
  uint8_t ones      = (sum%10);//parses the ones place

  //decoded numbers
  segment_data[0] = decoder(thousands);//thousands
  segment_data[1] = decoder(hundreds);//hundreds
  segment_data[2] = 0b11111111;//everyting off for colon
  segment_data[3] = decoder(tens);//tens
  segment_data[4] = decoder(ones);//ones

  //blank out leading zero digits
  // and move data to right place for misplaced colon position
  if ((thousands == 0) && (hundreds==0) && (tens ==0)) {// three leading zeros
    segment_data[0] = 0b11111111;
    segment_data[1] = 0b11111111;
    segment_data[3] = 0b11111111;
  }else if ((thousands==0) && (hundreds==0)) {// two leading zeros
    segment_data[0] = 0b11111111;
    segment_data[1] = 0b11111111;
  }else if (thousands==0){
    segment_data[0] = 0b11111111;//1 zero off
  }
}//segment_sum

/***********************************************************************
Function: update_count()
Description:
  This function will update the the counter value based on the buttons that are
  pressed.
Parameters:
  -uint16_t *cnt_val:
    This should be the current value of the counter. This value will be updated
    to reflec the increment of the button pushed.
Return:
  This will not return anything, but it updates the value that is passed to it.
  This is done because it passed as a pointer
************************************************************************/
void update_count(uint16_t *cnt_val){
  for (uint8_t n = 0; n < 8; n++) {//read in all buttons
    if(debounce_switch(n)){
      if (n ==0) {//button J8
        *cnt_val += 128;
      }else if (n==1) {//button J7
        *cnt_val +=64;
      }else if (n==2) {//button J6
        *cnt_val+=32;
      }else if (n==3) {//button J5
        *cnt_val +=16;
      }else if (n==4) {//button J4
        *cnt_val += 8;
      }else if (n==5) {//button J3
        *cnt_val+=4;
      }else if (n==6) {//button J2
        *cnt_val+=2;
      }else{//button J1
        *cnt_val+=1;
      }
    }
  }
}

int main()
{
DDRB = 0xF0;//set port bits 4-7 B as outputs
uint16_t cnt_val = 0;//counter to be displayed
uint8_t digit_sel = 0;// digit to be selected for display
while(1){
  _delay_ms(2);//insert loop delay for debounce
  DDRA = 0x00;//make PORTA an input Port
  PORTA = 0xFF;// pullups
  PORTB = 0xF0;//enable tristate buffer for pushbutton switches
  update_count(&cnt_val);//now check each button and increment the count as needed
  PORTB = 0xE0;//disable tristate buffer for pushbutton switches
  if (cnt_val > 1023) {//bound the count to 0 - 1023
    cnt_val = 0;
  }
  segsum(cnt_val);//break up the disp_value to 4, BCD digits in the array: call (segsum)

  if (digit_sel > 4) {//bound a counter (0-4) to keep track of digit to display
    digit_sel=0;
  }
  DDRA = 0xFF;//make PORTA an output
  PORTA = segment_data[digit_sel];//send 7 segment code to LED segments
  //send PORTB the digit to display
  if (digit_sel == 0) {
    PORTB = 0b01000000;//thousands digit
  }else if (digit_sel==1) {
    PORTB = 0b00110000;//hundreds digit
  }else if (digit_sel==2) {
    PORTB = 0b00100000;//colon digit
  }else if (digit_sel==3) {
    PORTB = 0b00010000;//tens digit
  }else if (digit_sel==4) {
    PORTB =0b00000000;//one digit
  }
  digit_sel++;//update digit to display(increment)
  }//while
  return 0;
}//main
