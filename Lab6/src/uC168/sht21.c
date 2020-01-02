#include <util/twi.h>
#include "sht21.h"
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

uint8_t sht21_wr_buf[2];
uint8_t sht21_rd_buf[3];

//Bit Macros
#define BIT0 0
#define BIT1 1
#define BIT2 2
#define BIT3 3
#define BIT4 4
#define BIT5 5
#define BIT6 6
#define BIT7 7


/*********************************************************
Function: read_temperature_SHT21()
Description:
  This function will tell the SHT21 Temperature sensor
  to take a reading. This will be returned in two bytes.
  The sensor is configured in the defualt mode:
  14-Bit mode
Parameters:
  NONE
Return:
  -uint16_t sht21_temperature:
    This variable holds the temperature value that was read
    from the SHT21 temperature sensor
***********************************************************/
uint16_t read_temperature_SHT21(){
  // sht21_wr_buf[0]=SHT21_SOFT_RESET;
  // twi_start_wr(SHT21_ADDRESS,sht21_wr_buf,1);//reset the system
  // _delay_ms(100);
  //Variable used to return temp value from read_temperature_SHT21I()
  uint16_t sht21_temperature;
  sht21_wr_buf[0]=SHT21_HOLD_TEMP;
  twi_start_wr(SHT21_ADDRESS,sht21_wr_buf,1);
  twi_start_rd(SHT21_ADDRESS,sht21_rd_buf,2);
  sht21_temperature = sht21_rd_buf[0];
  sht21_temperature = sht21_temperature<<8;//LS by 8
  sht21_temperature |= sht21_rd_buf[1];
  //set BIT0 and bit1 to zero for accurate covnersion
  sht21_temperature &= ~(1<<BIT0);
  sht21_temperature &= ~(1<<BIT1);

  return sht21_temperature;

}

/*********************************************************
Function: sht21_temp_convert()
Description:
  This function will take a tempearature reading, a Farenheit or Celcius
  flag, and a pointer to an array and then place the ASCII equivalent
  of the temperature reading in the array that is pointed to by the
  char pointer.
Parameters:
  - char temp_digits[]:
    This is the pointer that points to the array where the ASCII
    equivalent will be placed.
  - uint16_t sht21_temp:
    This is the temperature reading that is being passed to function
    as a value
  - uint8_t f_not_c:
      This is the flag that determines if the conversion is sent out
      as Farenheit or Celcius. 0x00=Celcius and 0x01=Farenheit
Return: VOID
***********************************************************/
void sht21_temp_convert(char temp_digits[], uint16_t sht21_temp, uint8_t f_not_c){
  float temperature = -46.85 + (175.72/65536)*(float)sht21_temp;//conversion formula from DS
  if (f_not_c) {//Convert farenheit
    temperature = (temperature*(9.0/5.0))+32;//Farenheit
    dtostrf(temperature,5,2,temp_digits);
    strcat(temp_digits,"F");//Add DEF F
  }else{//keep Celcius
    dtostrf(temperature,5,2,temp_digits);
    strcat(temp_digits, "C");//ADD DEG C
  }
}
