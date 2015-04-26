/**---------------------------------------------------------------------
 * \file uart.h
 * \author Peter Scheydt
 * \version 0.1
 * \brief 
 * 
 * -------------------------------------------------------------------*/

#ifndef UART_H 
#define UART_H 

//--------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

//--------------------------------------------------------------------------

#ifndef F_CPU
	#define F_CPU 1000000UL
#endif

#define UART_ASCII_MAXSTRLEN 100

#define BAUD 9600UL

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD)

//#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
	//#error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
//#endif 
	
#if defined (__AVR_ATmega88__) || (__AVR_ATmega48__)
	#define USR 		UCSR0A
	#define UCR 		UCSR0B
	#define UBRR 		UBRR0L
	#define TXEN 		TXEN0
	#define UDR 		UDR0
	#define UDRE 		UDRE0
	#define RXEN		RXEN0
	#define RXCIE		RXCIE0
	#define UCRC		UCSR0C
	#define USART_RX 	USART_RX_vect
#endif

#if defined (__AVR_ATmega644P__)
	#define USR 		UCSR0A
	#define UCR 		UCSR0B
	#define UBRR 		UBRR0L
	#define TXEN 		TXEN0
	#define UDR 		UDR0
	#define UDRE 		UDRE0
	#define RXEN		RXEN0
	#define RXCIE		RXCIE0
	#define UCRC		UCSR0C
	#define USART_RX 	USART0_RX_vect
#endif

//--------------------------------------------------------------------------

void uart_init(void);

int uart_put_uc(unsigned char c);

void uart_put_s (char *s);

void uart_get_data (void);

//--------------------------------------------------------------------------

#endif
