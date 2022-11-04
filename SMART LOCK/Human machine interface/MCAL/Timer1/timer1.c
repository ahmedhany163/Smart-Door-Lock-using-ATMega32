/******************************************************************************
 *
 * File Name: timer1.c
 *
 * Description: Source file for Timer1 Driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer1.h"
#include "../std_types.h"
/*******************************************************************************
 *                           Global Variables                                  *
 *******************************************************************************/

static volatile void (*g_callBackPtr)(void) = NULL_PTR;
static uint8 counter=0;
/*******************************************************************************
 *                       Interrupt Service Routines                            *
 *******************************************************************************/

ISR(TIMER1_COMPA_vect){
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
		counter=0;
	}
}

ISR(TIMER1_OVF_vect){
	if(g_callBackPtr != NULL_PTR)
	{
		(*g_callBackPtr)();
	}
}

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

void Timer1_init(const Timer1_ConfigType * Config_Ptr){

	//Enables Interrupt bit
	SREG |=(1<<7);

	/* WGM10 & WGM11 = 0 For Normal and Compare modes
	 * FOC1A and FOC1B for Non PWM mode
	 */
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);

	// Prescaler bits are inserted in CS10,CS11,CS12
	TCCR1B = (TCCR1B &= 0xF8) | (Config_Ptr->prescaler);

	// Initial value to start counting from
	TCNT1 = Config_Ptr->initial_value;

	// For Compare mode
	if(Config_Ptr->mode == Compare_Mode){
		/* Compare value to reset upon reaching
		 * 1 in OCIE1A to enable compare mode interrupt
		 * 1 in WGM12 to select comapre mode
		 */
		OCR1A = Config_Ptr->compare_value;
		TIMSK = (1<<OCIE1A);
		TCCR1B |= (1<<WGM12);
	}
	// For Normal mode
	else
	{
		// Turning on normal mode interrupt
		TIMSK =(1 << TOIE1);
	}
}

void Timer1_deInit(void){

	//Clearing all timer registers
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;
	OCR1A = 0;

	// Clearing all 4 Interrupt enable bits
	TIMSK &= (0xC3); //11000011
	counter=0;	// Reseting counter
}

void Timer1_setCallBack(void(*a_ptr)(void)){
	g_callBackPtr = a_ptr;
}
