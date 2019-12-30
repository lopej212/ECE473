// _delay_ms(1000);//wait one second for testing
// uart_putc('t');//for testing
// uart_putc('e');//for testing
// uart_putc('s');//for testing
// uart_putc('t');//for testing
// uart_putc('\0');//for testing
// _delay_ms(500);//for testing
// PORTB ^= (1<<5);//for testing
// _delay_ms(500);//for testing
// PORTB ^= (1<<5);//for testing
SHT21_temp_val = read_temperature_SHT21();//read sensor
// _delay_ms(500);//for testing
// PORTB ^= (1<<5);//for testing
// _delay_ms(500);//for testing
// PORTB ^= (1<<5);//for testing

if (rcv_rdy==1) {//packet received
  _delay_ms(1);
  _delay_ms(500);//for testing
  PORTB ^= (1<<5);//for testing
  _delay_ms(500);//for testing
  PORTB ^= (1<<5);//for testing
  rcv_rdy=0;//reset receive flag
  decode_rx_data();//decode data
  if (rx_array[0]=='s') {
    sample_flag=1;
  }
  if (sample_flag==1) {//Take temperature sample
    // _delay_ms(500);//for testing
    // PORTB ^= (1<<5);//for testing
    // _delay_ms(500);//for testing
    // PORTB ^= (1<<5);//for testing
    // uart_puts("-128.50C");//dummy data (for testing)
    read_SHT21();
    uart_puts(tx_array);
    uart_putc('\0');//send terminating character
    sample_flag=0;//reset flag

    _delay_ms(500);//for testing
    PORTB ^= (1<<5);//for testing
    _delay_ms(500);//for testing
    PORTB ^= (1<<5);//for testing
  }
}else{
  // _delay_ms(500);//for testing
  // PORTB ^= (1<<5);//for testing
  // _delay_ms(500);//for testing
  // PORTB ^= (1<<5);//for testing

}
