 /******************************************************************************
 *
 * File Name: dcmotor.h
 *
 * Description: Header file for the DC motor driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/
#ifndef DCMOTOR_H_
#define DCMOTOR_H_

#include "../../MCAL/std_types.h"
#include "../../MCAL/GPIO/gpio.h"

/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#define IN1_DC_Motor PORTB_ID,PIN0_ID
#define IN2_DC_Motor PORTB_ID,PIN1_ID
#define EN1_DC_Motor PORTB_ID,PIN2_ID

/*******************************************************************************
 *                     Types Declaration			                           *
 *******************************************************************************/

typedef enum
{
	CW,ACW
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void DcMotor_Init(void);
void DcMotor_Rotate(DcMotor_State state, uint8 speed);
void DcMotor_Stop();

#endif /* DCMOTOR_H_ */
