 /******************************************************************************
 *
 * File Name: timer1.h
 *
 * Description: Header file for Timer1 Driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/
#ifndef MCAL_TIMER1_TIMER1_H_
#define MCAL_TIMER1_TIMER1_H_

#include "../std_types.h"

extern uint8 Buzzer_flag;
extern uint8 Motor_flag;

/*******************************************************************************
 *                     Types Declaration			                           *
 *******************************************************************************/

typedef enum
{
Normal_Node, Compare_Mode
}Timer1_Mode;

typedef enum
{
No_CLK,CLK_1,CLK_8,CLK_64,CLK_256,CLK_1024,EXTERNAL_CLK_FALLING,EXTERNAL_CLK_RISING
}Timer1_Prescaler;

typedef struct
{
	uint16 initial_value;
	Timer1_Prescaler prescaler;
	Timer1_Mode mode;
	uint16 compare_value; // Used in compare only mode

}Timer1_ConfigType;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void Timer1_init(const Timer1_ConfigType * Config_Ptr);

void Timer1_deInit(void);

void Timer1_setCallBack(void(*a_ptr)(void));



#endif /* MCAL_TIMER1_TIMER1_H_ */
