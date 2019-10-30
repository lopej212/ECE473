#include <avr/io.h>
#include <util/delay.h>

#define OE_EN 7
#define SH_LD 6
#define CLK_INH 7

uint8_t value_decoder = 0;


//***********************************************************************
//                            spi_init
//**********************************************************************
void spi_init(void){

  DDRB   = 0x97; //output mode for SS, MOSI, SCLK, and output for OE_EN

  SPCR   = (1<<SPE) | (1<<MSTR) ; //master mode, clk low on idle, leading edge sample

  SPSR   = (1<<SPI2X); //choose double speed operation
 }

 uint8_t spi_read(void)
{
  SPDR = 0x00;
  while (bit_is_clear(SPSR,SPIF)) {}
  return(SPDR);
}

void decoder_read(){
  PORTE ^= (1<<SH_LD);//toggle to low
  PORTE ^= (1<<SH_LD);//toggle back to high
  PORTE ^= (1<<CLK_INH);//toggle to low
  value_decoder = spi_read();
  PORTE ^= (1<<CLK_INH);//toggle to high
}



 int main(){
   spi_init();
   DDRE = 0xC0;//ouput for LD and CLK_INH
   PORTE = 0xC0;//set them as HIGH
   PORTB |= (1<<OE_EN);//disable the bar graph (low enable)

   while(1){
     decoder_read();
     if (value_decoder==0xF3) {
       _delay_ms(500);
       PORTB ^= (1<<PB4);
       _delay_ms(500);
       PORTB ^= (1<<PB4);
     }
   }

   return 0;
 }
