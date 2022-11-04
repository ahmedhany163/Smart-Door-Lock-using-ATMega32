 /******************************************************************************
 *
 * File Name: buzzer.c
 *
 * Description: Source file for the Buzzer driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/
#define UART_START				0x01
#define UART_CONTINUE			0x11

#include "buzzer.h"
#include "../../MCAL/GPIO/gpio.h"
#include "../../MCAL/std_types.h"
#include "../../MCAL/UART/uart.h"

void Buzzer_Init(){
	GPIO_setupPinDirection(BUZZER_CONNECTION,PIN_OUTPUT);
	GPIO_writePin(BUZZER_CONNECTION,LOGIC_LOW);
}

void Buzzer_on(){
	GPIO_writePin(BUZZER_CONNECTION,LOGIC_HIGH);
}

void Buzzer_off(){
	GPIO_writePin(BUZZER_CONNECTION,LOGIC_LOW);
	UART_sendByte(UART_START);
	UART_sendByte(UART_CONTINUE);
}
