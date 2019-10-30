// lab2_ml.c
// Manuel Lopez Alcala


//  HARDWARE SETUP:
//  PORTA is connected to the segments of the LED display. and to the pushbuttons.
//  PORTA.0 corresponds to segment a, PORTA.1 corresponds to segement b, etc.
//  PORTB bits 4-6 go to a,b,c inputs of the 74HC138.
//  PORTB bit 7 goes to the PWM transistor base.

//#define F_CPU 16000000 // cpu speed in hertz
#include <avr/io.h>
#include <util/delay.h>

//holds data to be sent to the segments. logic zero turns segment on
uint8_t segment_data[5];

//decimal to 7-segment LED display encodings, logic "0" turns on segment
uint8_t dec_to_7seg[12];

//Array to hold butto values
uint8_t button_array[8] = {0};


//******************************************************************************
//                            debounce_switch()
//Checks the state of the button number passed to it. It shifts in ones till
//the button is pushed. Function returns a 1 only once per debounced button
//push so a debounce and toggle function can be implemented at the same time.
//Adapted to check all buttons from Ganssel's "Guide to Debouncing"
//Expects active low pushbuttons on PINA port.  Debounce time is determined by
//external loop delay times 12.
//
uint8_t debounce_switch(uint8_t pin) {
  static uint16_t state[8] = {0}; //holds present state
  state[pin] = (state[pin] << 1) | (! bit_is_clear(PINA, pin)) | 0xE000;
  if (state[pin] == 0xF000) return 1;
  return 0;
}
//******************************************************************************

/***************Function: decoder()***************************
Description:
  Takes in an 8 bit decimal int and returns the binary equivalent
  that will be used to display on the seven segement. bin_eq =ABCDEFG(DP)
Parameters:
  - uint8_t decimal: This is the decimal that needs to be converted
Precondition: NONE
Return: uint_t binary equivalent
************************************************************************/
uint8_t decoder(uint8_t decimal){
  uint8_t bin_eq = 0;
  switch (decimal) {
    case 0:
    {
      bin_eq=0b00000011;
      break;
    }
    case 1:
    {
      bin_eq=0b10011111;
      break;
    }
    case 2:
    {
      bin_eq=0b00100101;
      break;
    }
    case 3:
    {
      bin_eq=0b00001101;
      break;
    }
    case 4:
    {
      bin_eq=0b10011001;
      break;
    }
    case 5:
    {
      bin_eq=0b01001001;
      break;
    }
    case 6:
    {
      bin_eq=0b01000001;
      break;
    }
    case 7:
    {
      bin_eq=0b00011111;
      break;
    }
    case 8:
    {
      bin_eq=0b00000001;
      break;
    }
    case 9:
    {
      bin_eq=0b00001001;
      break;
    }
    default:
    {
      bin_eq = 0b11111111;//all segments are off
    }
  }
  return bin_eq;
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
  segment_data[0] = decoder(thousands);
  segment_data[1] = decoder(hundreds);
  segment_data[2] = 0b11111111;//everyting off for colon
  segment_data[3] = decoder(tens);
  segment_data[4] = decoder(ones);

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
  }else{
    asm volatile ("nop");
  }
  return;
}//segment_sum
//***********************************************************************************

/***************Function: display()***************************
Description:
  Sends 7 segment code to LED segments(PORTA) and sends PORTB the digit
  to display code
Parameters:
  -uint8_t digit_sel:
    This is the digit to be displayed.
          FORMAT:|digit0|digit1|colon(digit2)|digit3|digit4|
    segment_data:|   0  |  1   |      2      |  3   |   4  |
Precondition:
  -PORTA is set as output
Return: void
**************************************************************/
void display(uint8_t digit_sel){
  if (digit_sel == 0) {//digit 0(thousands)
    PORTA = segment_data[0];//(LEDs)
    PORTB = ((PB7<<1)|(PB6<<1)|(PB5<<0)|(PB4<<0));//0b11000000
  }else if (digit_sel == 1) {//digit 1 (hundreds)
    PORTA = segment_data[1];//LEDs
    PORTB = ((PB7<<1)|(PB6<<0)|(PB5<<1)|(PB4<<1));//0b10110000
  }else if (digit_sel == 2) {//colon
    PORTA = segment_data[2];//LEDs
    PORTB = ((PB7<<1)|(PB6<<0)|(PB5<<1)|(PB4<<0));//10100000
  }else if (digit_sel == 3) {// digit 3( tens)
    PORTA = segment_data[3];//LEDs
    PORTB = ((PB7<<1)|(PB6<<0)|(PB5<<0)|(PB4<<1));//0b10010000
  }else{
    PORTA = segment_data[4];// digit 4 (ones)
    PORTB = ((PB7<<1)|(PB6<<0)|(PB5<<0)|(PB4<<0));//0b10000000
  }
}


/**************************************************************
Function: check_buttons()
Description:
  This fucntion will check every button see if it is
  pressed. If it sees that it has been presed, then it processed
  to put a "1" in an array else it puts a "0". It then checks if
  the array and detects which button has been pressed. It then
  increments a temp varialb by the appropriate amount and returns the value.
	S1-> +1
	s2-> +2
	s3-> +4
	s4-> +8
	s5-> +16
	S6-> +32
	S7-> +64
	S8-> +128
Parameters:
  None
Return:
  -uint16_t varible that holds the value to increment the main
  coutner by.
*************************************************************/
void check_buttons()
{//

  for (uint8_t n = 0; n < 8; n++) {
    if (debounce_switch(n)) {
      if (n == 0) {
        button_array[0] = 1;
        //*value += 128;
      }else if (n == 1) {
        button_array[1] =1;
        //*value +=64;
      }else if (n == 2) {
        button_array[2]=1;
        //*value +=32;
      }else if (n==3) {
        button_array[3]=1;
        //*value +=16;
      }else if (n==4) {
        button_array[4]=1;
        //*value +=8;
      }else if (n==5) {
        button_array[5]=1;
        //*value +=4;
      }else if (n==6) {
        button_array[6]=1;
        //*value +=2;
      }else{
        button_array[7]=1;
        //*value |= 1<<n;
      }
    }
  }

// //check every button and put corresponding
// //1 or 0 in the array
//   for (uint8_t n = 0; n < 8; n++) {
//     if (debounce_switch(n)) {
//       button_array[n] = 1;
//     }else{
//       button_array[n] = 0;
//     }
//   }

  // if (button_array[0]) {//check A0(s8)
  //   *value += 1;
  //   //holder += 0x80;//d128
  //   // _delay_ms(500);//for testing
  //   // PORTB |= (1<<PB0);//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB |= (0<<PB0);//for testing
  //   //return 0x80;
  // }else if (button_array[1]) {//check A1(S7)
  //   *value += 1;
  //   //holder += 0x40;//d64
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //return holder;
  // }else if (button_array[2]) {//check A2(S6)
  //   *value += 1;
  //   //holder +=0x20;//d32
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //return holder;
  // }else if (button_array[3]) {//check A3(S5)
  //   *value += 1;
  //   //holder +=0x10;//dd16
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //return holder;
  // }else if (button_array[4]) {//check A4(S4)
  //   *value += 1;
  //   //holder +=0x08;//d8
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //return holder;
  // }else if (button_array[5]) {//check A5(S3)
  //   *value += 1;
  //   //holder +=0x04;//d4
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //eturn holder;
  // }else if (button_array[6]) {//check A6(S2)
  //   *value += 1;
  //   //holder +=0x02;//d2
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //return holder;
  // }else if (button_array[7]) {//check A7(S1)
  //   *value += 1;
  //   //holder +=0x01;//d1
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF1;//for testing
  //   // _delay_ms(500);//for testing
  //   // PORTB = 0xF0;//for testing
  //   //return holder;
  // }else{
  //   *value += 0;
  //   //holder +=0x00;//d0
  //   //return holder;
  // }
  // //return holder;
}

void updt_cnt(uint16_t * value){
  if (button_array[0] == 1) {
    *value +=128;
  }
}





//***********************************************************************************
uint8_t main()
{
DDRB = 0xFF;//set port bits 4-7 B as outputs
uint16_t cnt_val = 0;//counter variable
uint8_t  digit_sel   = 0x0;// LED display digit select


while(1){
  _delay_ms(1);//insert loop delay for debounce
  DDRA = 0x00;//make PORTA an input port
  PORTA = 0xFF;// activate pullups
  PORTB = 0xF0;//enable tristate buffer for pushbutton switches

  // for ( uint8_t n = 0; n <8; n++) {
  //   if (debounce_switch(n)) {
  //     counter_val += (1<<n);
  //   }
  // }


  check_buttons();//checks the buttons
  //updt_cnt(&counter_val);//update the counter

  if (cnt_val == 0) {
      _delay_ms(500);//for testing
      PORTB |= (1<<PB0);//for testing
      _delay_ms(500);//for testing
      PORTB |= (0<<PB0);//for testing
  }

  else if (button_array[0]==1) {
    cnt_val++;
    _delay_ms(500);//for testing
    PORTB |= (1<<PB1);//for testing
    _delay_ms(500);//for testing
    PORTB |= (0<<PB1);//for testing
  }

  // if (button_array[1] ==1 ) {
  //   _delay_ms(500);//for testing
  //   PORTB |= (1<<PB1);//for testing
  //   _delay_ms(500);//for testing
  //   PORTB |= (0<<PB1);//for testing
  // }

  // if (button_array[0] == 1) {
  //   _delay_ms(500);//for testing
  //   PORTB |= (1<<PB0);//for testing
  //   _delay_ms(500);//for testing
  //   PORTB |= (0<<PB0);//for testing
  // }

  // for (uint8_t k = 0; k < 8; k++) {
  //   button_array[k] = 0;
  // }




  // add_to_counter_val = check_buttons();
  // counter_val  = counter_val + add_to_counter_val;//check all the buttons and add to counter

  // if (test  == 0 ) {
  //   _delay_ms(500);//for testing
  //   PORTB |= (1<<PB0);//for testing
  //   _delay_ms(500);//for testing
  //   PORTB |= (0<<PB0);//for testing
  // }

  //counter_val += 1;//for testing

  // PORTB =0xE0; //(11100000)disable tristate buffer for pushbutton switches
  // if(counter_val > 1023)//bound the count to 0 - 1023
  // {
  //   counter_val = (counter_val-1023)-1;// reset counter
  // }
  // //break up the disp_value to 4, BCD digits in the array
  // segsum(counter_val);

  //bound a counter (0-4) to keep track of digit to display

  // DDRA = 0xFF;//make PORTA an output
  //
  // //display digit
  // display(digit_sel);
  //
  // //update digit to display
  // digit_sel++;
   }//while
}//main
