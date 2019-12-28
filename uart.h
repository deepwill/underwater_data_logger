/*
 * uart.h
 *
 *  Created on: Mar 1, 2018
 *      Author: andre
 */

#ifndef UART_H_
#define UART_H_



void uart_init(void);

void uart_putc(uint8_t data);

void uart_puts(char *s);
//uint8_t uart_receive( void );
void uart_transmit_hex( unsigned char dataType, unsigned long data );

#define TX_NEWLINE {uart_putc('\n'); uart_putc('\r');}


#endif /* UART_H_ */
