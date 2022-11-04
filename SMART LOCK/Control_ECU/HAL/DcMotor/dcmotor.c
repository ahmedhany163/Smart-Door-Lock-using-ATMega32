/******************************************************************************
 *
 * File Name: dcmotor.c
 *
 * Description: Source file for the DC motor driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/

#include "dcmotor.h"
#include "../../MCAL/common_macros.h"
#include "../../MCAL/Timer0_PWM/timer0_pwm.h"
#include "../../MCAL/UART/uart.h"

#define UART_START				0x01
#define UART_CONTINUE			0x11

void DcMotor_Init(void){
	GPIO_setupPinDirection(IN1_DC_Motor,PIN_OUTPUT);
	GPIO_setupPinDirection(IN2_DC_Motor,PIN_OUTPUT);
	GPIO_writePin(IN1_DC_Motor,LOGIC_LOW);
	GPIO_writePin(IN2_DC_Motor,LOGIC_LOW);
}


void DcMotor_Rotate(DcMotor_State state, uint8 speed){
	switch(state){
	case CW:
		GPIO_writePin(IN1_DC_Motor,LOGIC_HIGH);
		GPIO_writePin(IN2_DC_Motor,LOGIC_LOW);
		break;
	case ACW:
		GPIO_writePin(IN1_DC_Motor,LOGIC_LOW);
		GPIO_writePin(IN2_DC_Motor,LOGIC_HIGH);
		break;
	}

	switch(speed){
	case 25:
		Timer0_PWM_Init(64);
		break;
	case 50:
		Timer0_PWM_Init(128);
		break;
	case 100:
		Timer0_PWM_Init(255);
		break;
	}
}

void DcMotor_Stop(){
	GPIO_writePin(IN1_DC_Motor,LOGIC_LOW);
	GPIO_writePin(IN2_DC_Motor,LOGIC_LOW);
	UART_sendByte(UART_START);
}
