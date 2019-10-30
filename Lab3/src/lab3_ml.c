//Author: Jose Manuel Lopez Alcala
//ECE 473 Lab3
//Fall 2019
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//Decoder Macros
#define OE_EN 7
#define SH_LD 6
#define CLK_INH 7

//Bit Macros
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7

//Mode Variables
volatile uint8_t mode =0x00;
volatile uint8_t inc_dec1 = 1;
//holds data to be sent to the segments. logic zero turns segment on
volatile uint8_t segment_data[5];
//PORTC previous values
volatile uint8_t PORTC_previous = 0;
//PORTA previous value
volatile uint8_t PORTA_previous = 0;
//counter variable(for loop)
volatile uint8_t n = 0;
//Encoder 1 values
volatile uint8_t encoder1_past = 0;
volatile uint8_t encoder1_present =0;
//Encoder 2 value
volatile uint8_t encoder2_past = 0;
volatile uint8_t encoder2_present = 0;
//bit counter varibale
volatile uint8_t bitn = 0;
//reset bool variable
volatile uint8_t reset =1;// true when first start up
//Count variable
volatile int16_t ext_count = 0;//counter to be displayed
//encoder value
volatile uint8_t encoder_val = 0;

//******************************************************************************
//                            debounce_switch
//Checks the state of the button number passed to it. It shifts in ones till
//the button is pushed. Function returns a 1 only once per debounced button
//push so a debounce and toggle function can be implemented at the same time.
//Adapted to check all buttons from Ganssel's "Guide to Debouncing"
//Expects active low pushbuttons on PINA port.
//
uint8_t debounce_switch(uint8_t pin) {
  static uint16_t state[8] = {0};
  state[pin] = (state[pin]<<1)|(!bit_is_clear(PINA,pin))|0xE000;
  if (state[pin]==0xF000) {
    return 1;
  }
  return 0;
}

//***********************************************************************
//                            spi_init
//**********************************************************************
void spi_init(void){
  //DDRB should be 0x87, 0x97 is for testing
  DDRB   = 0x97; //output mode for SS, MOSI, SCLK, and output for OE_EN
  SPCR   = (1<<SPE) | (1<<MSTR) ; //master mode, clk low on idle, leading edge sample
  SPSR   = (1<<SPI2X); //choose double speed operation
 }

/***************************************************************
Function: spi_read()
Description:
  This function will read the SPI buffer and return the 8bit value
Parameters: NONE
Return:
  uint8_t value that contains the value of SPDR register
***************************************************************/
 uint8_t spi_read(void)
{
  SPDR = 0x00;
  while (bit_is_clear(SPSR,SPIF)) {}
  return(SPDR);
}

//***********************************************************************
//                           init_tcnt0
//***********************************************************************
//initalize timer/counter zero to normal mode
void init_tcnt0(){
  TIMSK |= (1<<TOIE0);//set interrupt for overflow
  TCCR0 |=  (1<<CS02) | (1<<CS00);  //normal mode, 128 prescale
}
//***********************************************************************

/***************************************************************
Function: update_inc_dec()
Description:
  This function will update the increase and decrese value
Parameters: NONE
Return:void
***************************************************************/
void update_inc_dec(){
  if (mode == 0) {// increas/decrease 1
    inc_dec1 = 1;
  }else if (mode == 1) {//increase/dec 2
    inc_dec1 = 2;
  }else if (mode == 2) {//inc/dec 4
    inc_dec1 = 4;
  }else if (mode == 3) {//inc/dec 0
    inc_dec1 = 0;
  }
}
/***************************************************************
Function: update_mode()
Description:
  This function will upate the mode of the system
Parameters: NONE
Return:void
***************************************************************/
void update_mode() {
  //Reconfigure the PORTS to read from the pushbuttons:
  DDRA = 0x00;//set PORT A to all inputs
  PORTA = 0xFF;//set PORTA all pullups
  PORTC = 0xF0;//power off to Seven segment and ENABLE TRI buffer

  for(n = 0; n<2;n++){//only check button 0 and 1
    if (debounce_switch(n)) {
      if (n == 0) {//button 0
        mode ^= (1<<BIT0);//increment by two mode (toggle bit 0)
      }else if (n==1) {//button 1
        mode ^= (1<<BIT1);//increment by four mode (toggle bit 1)
      }
    }
  }
  update_inc_dec();
  PORTC = 0xE0;//DISABLE Tri buffer (power still off to seven seg)
}


/***************************************************************
Function: read_encoders()
Description:
  This function will read the input of the encoder and increment
  or decrement the counta accordingly
Parameters: NONE
Return:void
***************************************************************/
void read_encoders(){
  PORTE ^= (1<<SH_LD);//toggle to low
  PORTE ^= (1<<SH_LD);//toggle back to high
  PORTE ^= (1<<CLK_INH);//toggle to low
  encoder_val= spi_read();
  PORTE ^= (1<<CLK_INH);//toggle to high

  //"clean" encoder1 value
  encoder1_present = encoder_val;
  for ( bitn = 7; bitn > 1; bitn--) {
    encoder1_present &= ~(1<<bitn);
  }
  //"clean" encoder2 value
  encoder2_present = (encoder_val>>2);//RS so that we get rid bit 0:1
  for ( bitn = 7; bitn > 1; bitn--) {
    encoder2_present &= ~(1<<bitn);
  }

  if (reset == 1) {
    encoder1_past = encoder1_present;//set them equal
    encoder2_past = encoder2_present;//set them equal
    reset = 0;//set to zero after first time
  }

  //Update for encoder1
  switch (encoder1_present) {
    case 1:
      if (encoder1_past == 0) {
        ext_count += inc_dec1;
        encoder1_past = encoder1_present;
      }else if (encoder1_past == 3 ) {
        ext_count -= inc_dec1;
        encoder1_past = encoder1_present;
      }
      break;
    case 3:
      if (encoder1_past == 1) {
        ext_count+= inc_dec1;
        encoder1_past = encoder1_present;
      }else if (encoder1_past == 2 ) {
        ext_count -=inc_dec1;
        encoder1_past = encoder1_present;
      }
      break;
    case 2:
      if (encoder1_past == 3) {
        ext_count += inc_dec1;
        encoder1_past = encoder1_present;
      }else if (encoder1_past == 0) {
        ext_count -= inc_dec1;
        encoder1_past = encoder1_present;
      }
      break;
    case 0:
      if (encoder1_past == 2) {
        ext_count += inc_dec1;
        encoder1_past = encoder1_present;
      }else if (encoder1_past == 1) {
        ext_count -= inc_dec1;
        encoder1_past = encoder1_present;
      }
      break;
    default:
      ext_count +=0;//add nothing
      break;
  }

  //Update for encoder 2
  switch (encoder2_present) {
    case 1:
      if (encoder2_past == 0) {
        ext_count += inc_dec1;
        encoder2_past = encoder2_present;
      }else if (encoder2_past == 3 ) {
        ext_count -= inc_dec1;
        encoder2_past = encoder2_present;
      }
      break;
    case 3:
      if (encoder2_past == 1) {
        ext_count+= inc_dec1;
        encoder2_past = encoder2_present;
      }else if (encoder2_past == 2 ) {
        ext_count -=inc_dec1;
        encoder2_past = encoder2_present;
      }
      break;
    case 2:
      if (encoder2_past == 3) {
        ext_count += inc_dec1;
        encoder2_past = encoder2_present;
      }else if (encoder2_past == 0) {
        ext_count -= inc_dec1;
        encoder2_past = encoder2_present;
      }
      break;
    case 0:
      if (encoder2_past == 2) {
        ext_count += inc_dec1;
        encoder2_past = encoder2_present;
      }else if (encoder2_past == 1) {
        ext_count -= inc_dec1;
        encoder2_past = encoder2_present;
      }
      break;
    default:
      ext_count +=0;//add nothing
      break;
  }
}


/***************************************************************
Function: ISR()
Description:
  This function will will take care of overflow timer/counter 0
  interrupt service routine
Parameters: NONE
Return:void
***************************************************************/
ISR(TIMER0_OVF_vect)
{
  //Store PORT values to be able to restore
  PORTA_previous =PINA;//save PORTA values
  PORTC_previous = PINC;//save PORTC values

  update_mode();//update mode
  read_encoders();//read encoders

  //update the count value
  if (ext_count>1023) {//bound count to 0-1023
    ext_count = (ext_count -1024);
  }
  if (ext_count<0) {
    ext_count = (1024+ext_count);
  }

  //restore PORT A and C values
  DDRA = 0xFF;//outside of ISR, always output
  PORTA = PORTA_previous;//restore PORTA
  PORTC = PORTC_previous;//restore PORTC

}

/***************************************************************
Function: display_mode()
Description:
  This function will display the mode on the bargraph
Parameters: NONE
Return:void
***************************************************************/
void display_mode(){
  SPDR = mode;//send value to register
  while(bit_is_clear(SPSR,SPIF)){}
  PORTB ^=(1<<BIT0);//send rising edge to regclk on HC595
  PORTB ^=(1<<BIT0);//send falling edge to reglck on HC595
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
void segsum(int16_t sum) {
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

int main()
{
  init_tcnt0();//initalize timer/counter 0
  spi_init();// initalize SPI protocol
  sei();//enable global interrupts
  DDRC = 0xF0;//set PORTC as output (never changes)
  DDRE = 0xC0;//set PORTE PIN 6 and 7 as ouputs(never changes)
  PORTE =0xC0;//set SH/LD and CLK_INH high (low enabled)
  uint8_t digit_sel = 0;//digit select


  while (1) {
    _delay_ms(2);
    display_mode();//display mode on bar graph
    segsum(ext_count);//break up the disp_value to 4, BCD digits in the array

    if (digit_sel > 4) {//bound a counter (0-4)
      digit_sel = 0;
    }
    DDRA = 0xFF;//make PORTA output for Seven segment
    PORTA = segment_data[digit_sel];//send 7 segment code to LED segments
    //send PORTB the digit to display & power ON to seven seg(bit7=0)
    if (digit_sel == 0) {
      PORTC = 0b01000000;//thousands digit
    }else if (digit_sel==1) {
      PORTC = 0b00110000;//hundreds digit
    }else if (digit_sel==2) {
      PORTC = 0b00100000;//colon digit
    }else if (digit_sel==3) {
      PORTC = 0b00010000;//tens digit
    }else if (digit_sel==4) {
      PORTC =0b00000000;//one digit
    }
    digit_sel++;//update digit to display(increment)
  }
  return 0;
}
