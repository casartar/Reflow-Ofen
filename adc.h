/*----------------------------------------------------------------------------
 Author:          	Peter Scheydt
 Remarks:        
 known Problems:  	none
 Version:         	17.10.2012
 Description:     	UART
 					Funktionen zur Kommunikation mittels UART 
 History:			
----------------------------------------------------------------------------*/
#ifndef ADC_H 
#define ADC_H
/*----------------------------------------------------------------------------*/
#include <avr/io.h> 
#include <avr/interrupt.h>
/*----------------------------------------------------------------------------*/
#define ADC_CHANNEL_START 0		//Hier wird definiert welche ADC-Kanäle über
#define ADC_CHANNEL_END	  5		//UART übertragen werden sollen
								//Diese müssen zusammenhängend sein
/*----------------------------------------------------------------------------*/
void 		adc_init		( void );

uint8_t 	ADC_Read		( uint8_t channel );

uint8_t 	ADC_Read_Avg	( uint8_t channel, uint8_t average );
/*----------------------------------------------------------------------------*/
#endif
