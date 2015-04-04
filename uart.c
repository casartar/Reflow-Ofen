/**---------------------------------------------------------------------
 * \file uart.c
 * \author Peter Scheydt
 * \version 0.1
 * \brief UART related functions
 * 
 * -------------------------------------------------------------------*/

#include "uart.h"

#define DEBUG 0


static volatile uint8_t 		uart_str_complete 							= 0; 
static volatile uint8_t 		uart_str_count 								= 0;
static volatile char 			uart_ascii_string[UART_ASCII_MAXSTRLEN + 1] = "";

/*----------------------------------------------------------------------------
 Name:          	uart_init
 Parameter:        	void
 return value:		void
 Version:         	03.12.2012
 Description:     	Diese Funktion initialisiert die UART
 					RX Complete Interrupt Enable
					Receiver Enable
					Transmitter Enable
					Zeichengröße 8 bit
----------------------------------------------------------------------------*/
void uart_init(void)
{ 
	UBRR = UBRR_VAL;
	UCR |= ( 1 << TXEN )   | ( 1 << RXEN ) | ( 1 << RXCIE );
	UCRC = ( 1 << UCSZ01 ) | ( 1 << UCSZ00 );
}
/*----------------------------------------------------------------------------
 Name:          	uart_put_uc
 Parameter:        	unsigned char
 return value:		int
 Version:         	03.12.2012
 Description:     	Diese Funktion sendet einen unsigned char (Buchstabe) 
 					über UART
----------------------------------------------------------------------------*/
int uart_put_uc(unsigned char c)
{ 
	while (!(USR & (1<<UDRE))) {} 	// warten bis Senden möglich
	UDR = c; 						// sende Zeichen
	return 0;
}
/*----------------------------------------------------------------------------
 Name:          	uart_put_s
 Parameter:        	char
 return value:		void
 Version:         	03.12.2012
 Description:     	Diese Funktion sendet einen string über UART 
----------------------------------------------------------------------------*/
void uart_put_s (char *s)
{ 
	while (*s)							// so lange *s != '\0'
	{ 
		uart_put_uc(*s);
		s++; 
	}
}

/**----------------------------------------------------------------------------
 * \brief          	uart_get_data
 * \param       		void
 * \return			void
 *
 * 
 * 					
----------------------------------------------------------------------------*/
void uart_get_data (void)
{
	if (uart_str_complete == 1)					//kompletter Befehl vorhanden
	{
		uart_str_complete = 0;
	}
}

/*----------------------------------------------------------------------------
 Name:          	ISR
 Parameter:        	USART_RX
 return value:		void
 Version:         	29.03.2013
 Description:     	Dieser Interrupt schreibt einen String in uart_ascii_string
----------------------------------------------------------------------------*/
ISR(USART_RX)
{ 
	unsigned char nextChar;
	nextChar = UDR0; 							// Daten aus dem Puffer lesen
	if( uart_str_complete == 0 ) 
	{ 											// wenn in Verwendung, Zeichen verwerfen
		if(	uart_str_count < UART_ASCII_MAXSTRLEN - 1 ) 
		{
			uart_ascii_string[uart_str_count++] = nextChar;
			if (nextChar == '\r')
			{
				uart_ascii_string[uart_str_count] = '\0';
				uart_str_count = 0;
				uart_str_complete = 1; 
			}
		}
		else if (nextChar == '\r')
		{
			uart_ascii_string[uart_str_count] = '\0';
			uart_str_count = 0;
		}
	}
	else if (nextChar != '\r')
	{ 

	}	
}
