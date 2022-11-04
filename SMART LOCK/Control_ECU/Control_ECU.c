/******************************************************************************
 *
 * File Name: Control_ECU.c
 *
 * Description: main file for the Control ECU
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/

#include <util/delay.h>
#include <avr/io.h>
#include "MCAL/GPIO/gpio.h"
#include "MCAL/UART/uart.h"
#include "MCAL/std_types.h"
#include "MCAL/TWI/twi.h"
#include "MCAL/Timer1/timer1.h"
#include "HAL/DcMotor/dcmotor.h"
#include "HAL/Buzzer/buzzer.h"
#include "HAL/EEPROM/external_eeprom.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/

#define UART_START				0x01
#define INCORRECT_PASSWORD		0x02
#define CORRECT_PASSWORD		0x03
#define OPEN_DOOR				0x04
#define CHANGE_PASSWORD			0x05
#define CLOSE_DOOR				0x06
#define SAME_PASSWORD			0x07
#define UART_NEW_PASSWORD_READY 0x08
#define PASSWORD_CHANGED		0x09
#define NEW_PASSWORD			0x10
#define UART_CONTINUE			0x11
#define UART_CLEAR				0x12

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * EEPROM_write_Password function takes the user given password and writes it
 * in the EEPROM
 */
void EEPROM_write_Password(uint8 *Str){
	uint8 counter;
	for(counter=0;counter<5;counter++){
		EEPROM_writeByte(0x01+counter, Str[counter]);
		_delay_ms(100);
	}
}

/*
 * EEPROM_read_Password function reads the password saved in EEPROM
 * and returns it in a given array
 */
void EEPROM_read_Password(uint8 *Str){
	uint8 EEPROM_Read_Byte;
	uint8 counter;
	for(counter=0;counter<5;counter++){
		EEPROM_readByte(0x01+counter, &EEPROM_Read_Byte);
		Str[counter]= EEPROM_Read_Byte;
		_delay_ms(100);
	}
}

/*
 * check_Password function compares two passwords to make sure they match
 */
uint8 check_Password(uint8 *Str1, uint8 *Str2){
	uint8 counter;
	for(counter=0;counter<5;counter++){
		if(Str1[counter] != Str2[counter]){
			return INCORRECT_PASSWORD;
		}
	}
	return CORRECT_PASSWORD;
}

int main(){


	/*******************************************************************************
	 *                           System Variables                                  *
	 *******************************************************************************/

	uint8 password1[5];
	uint8 password2[5];
	uint8 check;
	uint8 saved_Password[5];
	uint8 UART_received_Byte;
	uint8 incorrect_Passwords=0;

	/* UART Configuration
	 * 9600 Baud rate
	 * Parity bit OFF
	 * Double transmission speed
	 * Interrupt OFF
	 * One stop bit
	 */
	UART_ConfigType UART_Config = {9600, Parity_OFF, Double, UART_INT_OFF, One_bit};

	/*
	 * TWI Configuration
	 * Bit Rate: 400.000 Kbps using F_CPU=8MHz
	 * Prescaler/1
	 * Interrupt OFF
	 * Acknowledge bit OFF
	 */
	TWI_ConfigType TWI_Config = {0x02, 0x01, Rate_1, TWI_INT_OFF, ACK_OFF};

	/*
	 * Timer1 Configuration
	 * TCNT1 = 0:		Initial value
	 * Prescaler: 		F_CPU/256
	 * Operation mode: 	Compare mode
	 * OCR1A = 31250	Compare value
	 * Generates an interrupt every 1 second
	 */
	Timer1_ConfigType TIMER1_Config = {0,CLK_256,Compare_Mode,31250};

	/*******************************************************************************
	 *                           System Modules Initializations                    *
	 *******************************************************************************/
	UART_init(&UART_Config);
	TWI_init(&TWI_Config);
	DcMotor_Init();
	Buzzer_Init();
	while(1){

		/*
		 * Wait for HMI to send two passwords
		 */
		while(UART_recieveByte() != UART_START){} 			// UART beginning
		UART_receiveString(password1); 						// Receiving password1
		UART_receiveString(password2);						// Receiving password2
		/*
		 * Compare password1 and password2
		 */
		check= check_Password(password1, password2);
		_delay_ms(100);
		/*
		 * If passwords was not matched
		 */
		if(check == INCORRECT_PASSWORD){
			/*
			 * Send signal to HMI to identify that passwords did not match
			 */
			UART_sendByte(UART_START);
			UART_sendByte(INCORRECT_PASSWORD);				// Incorrect password signal
		}
		/*
		 * If passwords match
		 */
		else if(check == CORRECT_PASSWORD){
			/*
			 * Send signal to HMI to identify that passwords matched
			 */
			UART_sendByte(UART_START);
			UART_sendByte(CORRECT_PASSWORD);				// Correct password signal

			// Save password in EEPROM
			EEPROM_write_Password(password1);
			// Getting saved password from EEPROM
			EEPROM_read_Password(saved_Password);

			/*
			 * System begins
			 */
			while(1){

				Timer1_deInit();							// De-Initialize timer1

				/*
				 * Password protection, if password is entered incorrectly 3 times
				 * Display ERROR, Initiate buzzer, Freeze system
				 */
				if(incorrect_Passwords==3){
					Buzzer_on();
					Buzzer_flag=1;							// Raise buzzer flag to identify in Timer1 ISR
					Timer1_setCallBack(Buzzer_off);			// Setting Timer1 call back function to turn of the buzzer
					Timer1_init(&TIMER1_Config);			// Initialize Timer1
					incorrect_Passwords=0;					// Reset trials
				}

				/*
				 * Wait for HMI start
				 */
				while(UART_recieveByte() != UART_START){}
				/*
				 * Receive input from HMI
				 */
				UART_received_Byte = UART_recieveByte();
				/*
				 * If input was Open Door
				 */
				if(UART_received_Byte== OPEN_DOOR)
				{
					/*
					 * Receive password and compare it with stored password in EEPROM
					 */
					UART_receiveString(password1);
					check = check_Password(password1, saved_Password);
					/*
					 * if Check was incorrect
					 */
					if(check==INCORRECT_PASSWORD){
						UART_sendByte(UART_START);
						UART_sendByte(INCORRECT_PASSWORD);			// Incorrect password signal
						incorrect_Passwords++;						// Counting incorrect trials
						continue;
					}
					/*
					 * If check was correct
					 */
					else if(check == CORRECT_PASSWORD){
						incorrect_Passwords=0;					// Reset incorrect trials

						UART_sendByte(UART_START);
						UART_sendByte(CORRECT_PASSWORD);			// Correct password signal
						/*
						 * Initialize Dc-Motor, Initialize Timer1
						 * Set Timer1 callback function to stop the door when it opens completely
						 * Raise Motor flag to identify in Timer1 ISR
						 */
						DcMotor_Rotate(CW, 100);
						Timer1_setCallBack(DcMotor_Stop);
						Motor_flag=1;
						Timer1_init(&TIMER1_Config);
						UART_sendByte(UART_CLEAR);

						/*
						 * Wait for close door signal
						 */
						while(UART_recieveByte() != CLOSE_DOOR){}
						Timer1_deInit();							// Reset Timer1
						/*
						 * Closing door mechanism
						 */
						DcMotor_Rotate(ACW, 100);
						Timer1_setCallBack(DcMotor_Stop);
						Motor_flag=1;
						Timer1_init(&TIMER1_Config);
						while(UART_recieveByte() != UART_CONTINUE){}

					}
				}

				/*
				 * If input was Change Password
				 */
				else if (UART_received_Byte == CHANGE_PASSWORD)
				{
					/*
					 * Receive password and compare it with saved password in EEPROM
					 */
					UART_receiveString(password1);
					check = check_Password(password1, saved_Password);

					/*
					 * If check was incorrect
					 */
					if(check==INCORRECT_PASSWORD){
						UART_sendByte(UART_START);
						UART_sendByte(INCORRECT_PASSWORD);			// Incorrect password signal
						incorrect_Passwords++;						// Counting incorrect trials
						continue;
					}

					/*
					 * If check was correct
					 */
					else if(check == CORRECT_PASSWORD){
						incorrect_Passwords=0;					// Reset incorrect trials
						UART_sendByte(UART_START);
						UART_sendByte(CORRECT_PASSWORD);			// Correct password signal

						/*
						 * Wait for new password from HMI
						 */
						while(UART_recieveByte() != UART_NEW_PASSWORD_READY){}
						/*
						 * Receive password and check if it matches old password or not
						 */
						UART_receiveString(password1);

						check = check_Password(password1, saved_Password);
						/*
						 * If passwords did not match
						 * Change password complete
						 */
						if(check == INCORRECT_PASSWORD){
							UART_sendByte(UART_START);
							UART_sendByte(NEW_PASSWORD);
							EEPROM_write_Password(password1);
							EEPROM_read_Password(saved_Password);
							UART_sendByte(PASSWORD_CHANGED);
							continue;
						}
						/*
						 * If passwords match
						 * Same password was entered
						 * Do nothing
						 */
						else if(check == CORRECT_PASSWORD){
							UART_sendByte(UART_START);
							UART_sendByte(SAME_PASSWORD);
						}
					}
				}
			}
		}
	}
	return 0;
}
