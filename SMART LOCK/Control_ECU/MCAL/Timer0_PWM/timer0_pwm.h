 /******************************************************************************
 *
 * File Name: timer0_pwm.h
 *
 * Description: header file for Timer0 PWM Driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/
#ifndef TIMER_H_
#define TIMER_H_
#include "../std_types.h"
#include "../GPIO/gpio.h"

void Timer0_PWM_Init(uint8 duty_cycle);

#endif /* TIMER_H_ */
