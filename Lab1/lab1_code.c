// lab1_code.c
// R. Traylor
// 7.21.08
// Edited for Assignment 1 by Manuel Lopez


//This program increments a binary display of the number of button pushes on switch
//S0 on the mega128 board.

#include <avr/io.h>
#include <util/delay.h>

//*******************************************************************************
//                            debounce_switch
// Adapted from Ganssel's "Guide to Debouncing"
// Checks the state of pushbutton S0 It shifts in ones till the button is pushed.
// Function returns a 1 only once per debounced button push so a debounce and toggle
// function can be implemented at the same time.  Expects active low pushbutton on
// Port D bit zero.  Debounce time is determined by external loop delay times 12.
//*******************************************************************************
int8_t debounce_switch() {
  static uint16_t state = 0; //holds present state
  state = (state << 1) | (! bit_is_clear(PIND, 0)) | 0xE000;
  if (state == 0xF000) return 1;
  return 0;
}

//******************************************************************************
//Function name: convert_to_bcd()
//Description:
//	This function will take a value and return a the BCD of the number using
//	8 leds
//Precondition:
//	None
//Parameters:
//	* uint8_t value:
//		This is the value that gets converted to the BCD equivalent.
//		The value should be an 8 bit number.
//Return:
//	Returns the binary-coded decimal
//******************************************************************************
uint8_t convert_to_bcd(uint8_t value){
	uint8_t bcd_value = 0;//Variable to hold coverted bcd_value

	if(value < 10){//if value is 0-9
		bcd_value = value;
	}
	else if (value < 20){//if value is 10-19
		bcd_value = value + 6;
	}
	else if ( value <30){//if value is 20-29
		bcd_value = value +12;
	}
	else if (value < 40){//if value is 30-39
		bcd_value = value +18;
	}
	else if (value < 50){//if value is 40-49
		bcd_value = value + 24;
	}
	else if ( value < 60){//if value is 50-59
		bcd_value = value + 30;
	}
	else if ( value < 70){//if value is 60-69
		bcd_value = value + 36;
	}
	else if (value < 80){//if value is 70-79
		bcd_value = value + 42;
	}
	else if (value < 90){//if value is 80-89
		bcd_value = value + 48;
	}
	else{//else the value is 90-99
		bcd_value = value + 54;
	}

	return bcd_value;
}

//*******************************************************************************
// Check switch S0.  When found low for 12 passes of "debounce_switch(), increment
// PORTB.  This will make an incrementing count on the port B LEDS.
//*******************************************************************************
int main()
{
DDRB = 0xFF;  //set port B to all outputs
uint8_t value =0;// variable to hold the value is pushes

while(1){     //do forever
 if(debounce_switch()) {
	 value++;
	 if(value > 99){
		 value =0;//reset the count
	 }
         PORTB = convert_to_bcd(value);//Set PORTB equal to BCD value
	}
  _delay_ms(2);                    //keep in loop to debounce 24ms
  } //while
} //main
