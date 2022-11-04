 /******************************************************************************
 *
 * File Name: buzzer.h
 *
 * Description: Header file for the Buzzer driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/



#ifndef HAL_BUZZER_BUZZER_H_
#define HAL_BUZZER_BUZZER_H_


/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/

#define BUZZER_CONNECTION		PORTB_ID,PIN5_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

void Buzzer_Init();

void Buzzer_on();

void Buzzer_off();

#endif /* HAL_BUZZER_BUZZER_H_ */
