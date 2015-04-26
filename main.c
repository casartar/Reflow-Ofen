#define F_CPU 1000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd-routines.h"
#include "debounce.h"
#include "adc.h"
#include "uart.h"
#include "debounce.h"

#define PHASE_IDLE	0
#define PHASE_1		1	// Heat up 1
#define PHASE_2		2	// Heat up 2
#define PHASE_3		3	// Reflow 1
#define PHASE_4		4	// Reflow 2
#define PHASE_5		5	// Cool down

#define RELAIS_PORT		PORTD 
#define RELAIS_DDR		DDRD
#define RELAIS			PD7

#define BUTTON_START_PORT		PORTD 
#define BUTTON_START_DDR		DDRD
#define BUTTON_START			PD5
#define BUTTON_START_PIN		PIND

void timer1_init(void);
uint8_t set_relais(uint16_t setPoint, int32_t temperature);
void display_values (uint16_t time_step,uint16_t setPoint, int32_t temperature);

uint8_t start_flag = 0;

int main (void)
{
	adc_init();
	timer1_init();
	uart_init();
	lcd_init();
	lcd_clear();
	lcd_string("Bereit");
	uart_put_s("Step;Soll (deg);Ist (deg)\r\n");
	// CSV-format
	RELAIS_DDR |= (1 << RELAIS);
	BUTTON_START_DDR &= ~((1 << BUTTON_START));
	BUTTON_START_PORT |= (1 << BUTTON_START);
	DDRD |= (1 << PD6);
	sei();
	while(1)
	{
		// Evaluate button	-	start/stop Button
		if (debounce(BUTTON_START_PIN,BUTTON_START))
		{
			if (start_flag)
			{
				// Oven is active - Not IDLE
				start_flag = 0;
				lcd_clear();
				lcd_string("Stop   ");	// Shows up short - until next tick
			}
			else
			{
				// Oven is IDLE
				start_flag = 1;
				lcd_clear();
				lcd_string("Start  ");	// // Shows up short - until next tick
			}
		}
	}
}

void timer1_init(void)
{
	// 1 second tick
	TIMSK1 |= (1 << TOIE1);
	//Prescaler 1024 
	TCCR1B |= (1<<CS10 | 0<<CS11 | 1<<CS12);
	TCNT1 = 64560;
}

uint8_t set_relais(uint16_t setPoint, int32_t temperature)
{
	if (setPoint < temperature)
	{
		RELAIS_PORT &= ~(1 << RELAIS);
		return 0;
	}
	else
	{
		RELAIS_PORT |= (1 << RELAIS);
		return 1;
	}
}

void display_values (uint16_t time_step,uint16_t setPoint, int32_t temperature)
{
	char Buffer[20];
	sprintf(Buffer,"%hu;%hu;%ld\r\n",time_step,setPoint,temperature);
	uart_put_s(Buffer);
	
	lcd_setcursor(9,1);
	sprintf(Buffer,"Ist:%03ld",temperature);
	lcd_string(Buffer);
	
	lcd_setcursor(0,2);
	sprintf(Buffer,"TS:%03hu  Soll:%03hu",time_step,setPoint);
	lcd_string(Buffer);
}


ISR (TIMER1_OVF_vect)
{
	static volatile uint16_t 	time_step 	= 0;
	static volatile uint8_t 	phase 		= 0;
	uint16_t 	setPoint 	= 0;
	int32_t 	temperature = 0;
	
	PORTD ^= (1 << PD6);	// Debug
	
	//724.62 x-503.723
	temperature = ADC_Read_Avg	( 5, 32 );
	temperature = (temperature*275)/64;		//mV
	temperature = ((-temperature*724)+503000)/1000;	//°C
	
	switch (phase)
	{
		case PHASE_IDLE:
			if (start_flag)
			{
				phase = PHASE_1;
				lcd_home();
				lcd_string("Phase 1");
				time_step = 0;
			}
			// Temperature update all the time
			lcd_setcursor(9,1);
			char Buffer[20];
			sprintf(Buffer,"Ist:%03ld",temperature);
			lcd_string(Buffer);
		break;
		case PHASE_1:
			setPoint = 20 + time_step;
			set_relais(setPoint, temperature);
			display_values(time_step,setPoint, temperature);
			if (time_step >= 155)
			{
				phase = PHASE_2;
				time_step = 0;
				lcd_home();
				lcd_string("Phase 2");
			}
			if (!start_flag)
			{
				// abort condition
				phase = PHASE_IDLE;
				RELAIS_PORT &= ~(1 << RELAIS);
				lcd_clear();
				lcd_string("Bereit");
			}
		break;
		case PHASE_2:
			setPoint = 175 + (time_step*375)/1000;
			set_relais(setPoint, temperature);
			display_values(time_step,setPoint, temperature);
			if (time_step >= 40)
			{
				phase = PHASE_3;
				time_step = 0;
				lcd_home();
				lcd_string("Phase 3");
			}
			if (!start_flag)
			{
				// abort condition
				phase = PHASE_IDLE;
				RELAIS_PORT &= ~(1 << RELAIS);
				lcd_clear();
				lcd_string("Bereit");
			}
		break;
		case PHASE_3:
			setPoint = 190 + (time_step*30)/17;
			set_relais(setPoint, temperature);
			display_values(time_step,setPoint, temperature);
			if (time_step >= 38)
			{
				phase = PHASE_4;
				time_step = 0;
				lcd_home();
				lcd_string("Phase 4");
			}
			if (!start_flag)
			{
				// abort condition
				phase = PHASE_IDLE;
				RELAIS_PORT &= ~(1 << RELAIS);
				lcd_clear();
				lcd_string("Bereit");
			}
		break;
		case PHASE_4:
			setPoint = 250;
			set_relais(setPoint, temperature);
			display_values(time_step,setPoint, temperature);
			if (time_step >= 10)
			{
				phase = PHASE_5;
				time_step = 0;
				RELAIS_PORT &= ~(1 << RELAIS);
				lcd_home();
				lcd_string("Phase 5");
			}
			if (!start_flag)
			{
				// abort condition
				phase = PHASE_IDLE;
				RELAIS_PORT &= ~(1 << RELAIS);
				lcd_clear();
				lcd_string("Bereit");
			}
		break;
		case PHASE_5:
			// The oven is deactivated 
			// All conditions are displayed until temperature is below 20°C
			setPoint = 20;	// Not necessary
			display_values(time_step,setPoint, temperature);
			if (temperature <= 20 || !start_flag) 
			{
				phase = PHASE_IDLE;
				RELAIS_PORT &= ~(1 << RELAIS);
				lcd_clear();
				lcd_string("Bereit");
				start_flag = 0;		// In case abortion by low temperature
			}
		break;
	}
	time_step++;
	TCNT1 = 64560;		//65.535
}
