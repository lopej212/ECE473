
#ifndef UART_H_
#define UART_H_

void usart_inc()
{
	UBRRL=0x67;
	UBRRH=0x00;
	UCSRB|=(1<<TXEN);
	UCSRC=(1<<UCSZ1)|(1<<UCSZ0)|(1<<URSEL);
}

void usart_send_char(unsigned char c)
{
	while(!UCSRA&(1<<UDRE));
	UDR=c;
}

void usart_send_string(unsigned char data[])
{
	while(*data!=NULL)
	{
		usart_send_char(*data);
		data++;
		_delay_ms(100);
	}
}

#endif /* UART_H_ */