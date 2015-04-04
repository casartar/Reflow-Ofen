#include "adc.h"

/*----------------------------------------------------------------------------
 Name:          	adc_init
 Parameter:        	void
 return value:		void
 Version:         	29.03.2013
 Description:     	Diese Funktion initialisiert den ADC 					
----------------------------------------------------------------------------*/
void adc_init( void ) 
{ // ADC initialisieren
	uint8_t result;
	ADMUX  |= (1<<REFS1)|(1<<REFS0); 	// 1,1V als Referenz benutzen
	ADMUX  |= (1<<ADLAR);				// Ober 8bit des AVC stehen im High-Byte
	ADCSRA  = (1<<ADPS2) | (1<<ADPS1); 	// Frequenzvorteiler 64
	ADCSRA |= (1<<ADEN); 				// ADC aktivieren
	ADCSRA |= (1<<ADSC); 				// erste ADC-Wandlung starten
	while (ADCSRA & (1<<ADSC) ) {} 		// Warten bis Wandlung fertig
	result = ADCW; 						// Ergebnis holen und verwerfen
}
/*----------------------------------------------------------------------------
 Name:          	ADC_Read
 Parameter:        	uint8_t
 return value:		uint8_t
 Version:         	29.03.2013
 Description:     	Diese Funktion stellt die 8 höchstwertigen Bits eines 
 					ADC-Kanals bereit 					
----------------------------------------------------------------------------*/
uint8_t ADC_Read( uint8_t channel ) 				// ADC Einzelmessung
{ 
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F); 	// Kanal wählen
	ADCSRA |= (1<<ADSC); 							// eine Wandlung "single conversion"
	while (ADCSRA & (1<<ADSC) ) {} 					// auf Ende der Konvertierung warten
	return ADCH; 									// ADC auslesen und zurückgeben
}
/*----------------------------------------------------------------------------
 Name:          	ADC_Read_Avg
 Parameter:        	uint8_t
 return value:		uint8_t , uint8_t
 Version:         	29.03.2013
 Description:     	Diese Funktion bildet aus mehrfachen ADC-Messungen einen 
 					Mittelwert					
----------------------------------------------------------------------------*/
uint8_t ADC_Read_Avg( uint8_t channel, uint8_t average )
{ 
	uint32_t result = 0;
	for (uint8_t i = 0; i < average; ++i )
	result += ADC_Read( channel );
	return (uint8_t)( result / average );
}
