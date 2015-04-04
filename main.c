#define F_CPU 8000000UL
#include <avr/io.h> 
#include <util/delay.h>
#include "uart.h"
#include "adc.h"
#include <stdio.h>

int main (void)
{
	uart_init();
	adc_init();
	char Buffer[20];
	while(1)
	{
		
		sprintf(Buffer, "%u \n", ADC_Read_Avg(5, 32)*43/10);
		uart_put_s(Buffer);
		_delay_ms(500);
	}
}
