#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"

//todo: name convention same: usart_transmit, usart_receive, usart_transmithex, usart

void uart_init(void)
{
//	DDRD |= (1<<PD1);	//Set TXD (PD1) as output for USART
	unsigned int USART_BAUDRATE = 9600;		
	unsigned int ubrr = (((F_CPU / (USART_BAUDRATE * 16UL)))-1 );	//baud prescale calculated according to F_CPU-define at top

	/*Set baud rate */
#if defined(__AVR_ATmega8A__)
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	/*	Enable receiver and transmitter */
	UCSRB = (1<<TXEN);//(1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit, The two stop-bits does not seem to make any difference in my case!?*/
	//UCSRC = (1<<USBS)|(3<<UCSZ0);
	UCSRC |= (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); //8n1

#endif // __AVR_ATmega8__


#if defined(__AVR_ATmega88P__)
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*	Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit, The two stop-bits does not seem to make any difference in my case!?*/
	//UCSRC = (1<<USBS)|(3<<UCSZ0);
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); //8n1
#endif // __AVR_ATmega88P__

#if defined(__AVR_ATmega88__)
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*	Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit, The two stop-bits does not seem to make any difference in my case!?*/
	//UCSRC = (1<<USBS)|(3<<UCSZ0);
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); //8n1
#endif // __AVR_ATmega88P__

}


void uart_putc(uint8_t data)
{
	/* Wait for empty transmit buffer */
#if defined(__AVR_ATmega88P__)
	while ( !(UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
#endif

#if defined(__AVR_ATmega88__)
	while ( !(UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
#endif

#if defined(__AVR_ATmega8A__)
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
    UDR=data;
#endif
}

void uart_puts(char *s)
{ /* send string per UART
*/
	while(*s)
	{
		uart_putc(*s);
		s++;
	}
}



//uint8_t uart_receive( void )
//{
//#if defined(__AVR_ATmega88P__)
//	/* Wait for data to be received */
//	while ( !(UCSR0A & (1<<RXC0)) );	//This loop is only needed if you not use the interrupt...
//	/* Get and return received data from buffer */
//	return UDR0; //Return the received byte
//#endif
//
//#if defined(__AVR_ATmega88__)
//	/* Wait for data to be received */
//	while ( !(UCSR0A & (1<<RXC0)) );	//This loop is only needed if you not use the interrupt...
//
//	/* Get and return received data from buffer */
//	return UDR0; //Return the received byte
//#endif
//
//#if defined(__AVR_ATmega8A__)
//	/* Wait for data to be received */
//	while ( !(UCSRA & (1<<RXC)) );	//This loop is only needed if you not use the interrupt...
//
//	/* Get and return received data from buffer */
//	return UDR; //Return the received byte
//#endif
//
//}


//***************************************************
//Function to transmit hex format data
//first argument indicates type: CHAR, INT or LONG
//Second argument is the data to be displayed
//***************************************************
/*void uart_transmit_hex( unsigned char dataType, unsigned long data )
{
unsigned char count, i, temp;
unsigned char dataString[] = "0x        ";

if (dataType == 'C') count = 2;
if (dataType == 'I') count = 4;
if (dataType == 'L') count = 8;

for(i=count; i>0; i--)
{
  temp = data % 16;
  if((temp>=0) && (temp<10)) dataString [i+1] = temp + 0x30;
  else dataString [i+1] = (temp - 10) + 0x41;

  data = data/16;
}

uart_puts (dataString);
}*/
