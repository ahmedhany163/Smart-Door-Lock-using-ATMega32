/******************************************************************************
 *
 * File Name: HMI.c
 *
 * Description: main file for the Human Machine Interface
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/
#include <util/delay.h>
#include "MCAL/std_types.h"
#include "MCAL/UART/uart.h"
#include "HAL/Keypad/keypad.h"
#include "HAL/LCD/lcd.h"

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

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * get_Password function takes a password of 5 digits from user
 * and modifies to make it ready for transmission via UART
 */
void get_Password(uint8 *Str){
	uint8 counter;

	for(counter=0;counter<5;counter++){
		Str[counter] = KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		_delay_ms(500);
	}
	while(KEYPAD_getPressedKey()!='=');
	_delay_ms(300);
	Str[5]= '#';				// To be able to send via UART

}

int main(void){


	/*******************************************************************************
	 *                           System Variables                                  *
	 *******************************************************************************/
	uint8 password1[6];
	uint8 password2[6];
	uint8 incorrect_Passwords=0;
	uint8 KEYPAD_Key_Pressed;
	uint8 UART_received_Byte;

	/* UART Configuration
	 * 9600 Baud rate
	 * Parity bit OFF
	 * Double transmission speed
	 * Interrupt OFF
	 * One stop bit
	 */
	UART_ConfigType UART_Config = {9600, Parity_OFF, Double, UART_INT_OFF,One_bit};

	/*******************************************************************************
	 *                           System Modules Initializations                    *
	 *******************************************************************************/
	UART_init(&UART_Config);
	LCD_init();
	while(1){
		// Asking the user for first password
		LCD_clearScreen();
		LCD_moveCursor(0, 0);
		LCD_displayString("Enter Password:");
		LCD_moveCursor(1, 0);

		get_Password(password1);		// Get first password

		// Asking the user for second password
		LCD_clearScreen();
		LCD_moveCursor(0, 0);
		LCD_displayString("Please Re-Enter");
		LCD_moveCursor(1, 0);
		LCD_displayString("Password:");
		LCD_moveCursor(1, 9);

		get_Password(password2);		// Get second password

		UART_sendByte(UART_START);							// UART beginning
		UART_sendString(password1);							// Sending password1 to CONTROL_ECU
		UART_sendString(password2);							// Sending password2 to CONTROL_ECU

		while(UART_recieveByte() != UART_START){}			// Waiting for response on passwords matching
		UART_received_Byte = UART_recieveByte();
		/*
		 * If passwords are not matched display a message then return to
		 * take new passwords
		 */

		if(UART_received_Byte == INCORRECT_PASSWORD){
			LCD_clearScreen();
			LCD_moveCursor(0, 0);
			LCD_displayString("Incorrect");
			LCD_moveCursor(1, 0);
			LCD_displayString("Password!");
			_delay_ms(1000);
			continue;
		}
		/*
		 * If passwords match, password is saved and system features unlock
		 */
		else if(UART_received_Byte == CORRECT_PASSWORD)
		{
			// Password set, System features unlocked
			LCD_clearScreen();
			LCD_moveCursor(0, 0);
			LCD_displayString("New Password set");
			LCD_moveCursor(1, 0);
			LCD_displayString("Successfully");
			_delay_ms(1000);

			/*
			 * System begins
			 */
			while(1){

				/*
				 * Password protection, if password is entered incorrectly 3 times
				 * Display ERROR, Initiate buzzer, Freeze system
				 */
				if(incorrect_Passwords==3){
					LCD_clearScreen();
					LCD_moveCursor(0, 5);
					LCD_displayString("ERROR!");
					while(UART_recieveByte() != UART_START){}
					UART_received_Byte = UART_recieveByte();
					if(UART_received_Byte == UART_CONTINUE){
						incorrect_Passwords=0;				// Reset trials
					}
				}

				/*
				 * Main Screen options
				 */
				LCD_clearScreen();
				LCD_moveCursor(0, 0);
				LCD_displayString("+ : Open Door");
				LCD_moveCursor(1, 0);
				LCD_displayString("- : Change Pass");
				KEYPAD_Key_Pressed = KEYPAD_getPressedKey();
				_delay_ms(500);

				/*
				 *  Open Door functions
				 */
				if(KEYPAD_Key_Pressed == '+'){
					/*
					 * Ask user for password
					 */
					LCD_clearScreen();
					LCD_moveCursor(0, 0);
					LCD_displayString("Enter Password:");
					LCD_moveCursor(1, 13);
					LCD_displayCharacter('(');
					LCD_intgerToString(incorrect_Passwords+1);
					LCD_displayCharacter(')');
					LCD_moveCursor(1, 0);

					get_Password(password1);		// Get password

					/*
					 * Send password to CONTROL_ECU to check if it matches
					 */
					UART_sendByte(UART_START);
					UART_sendByte(OPEN_DOOR);
					UART_sendString(password1);

					/*
					 * Wait for response from CONTORL_ECU
					 */
					while(UART_recieveByte() != UART_START){}
					UART_received_Byte = UART_recieveByte();

					/*
					 * Incorrect password
					 */
					if(UART_received_Byte == INCORRECT_PASSWORD){	// Passwords were not matched
						LCD_clearScreen();
						LCD_moveCursor(0, 0);
						LCD_displayString("Incorrect");
						LCD_moveCursor(1, 0);
						LCD_displayString("Password!");
						incorrect_Passwords++;						// Counting incorrect trials
						_delay_ms(1000);
						continue;
					}

					/*
					 * Correct password
					 */
					else if(UART_received_Byte == CORRECT_PASSWORD) {
						incorrect_Passwords=0;					// Reset incorrect trials
						/*
						 * Door Opening
						 */
						LCD_clearScreen();
						LCD_moveCursor(0, 0);
						LCD_displayString("Door is Opening");
						/*
						 * Waiting until door opens completely
						 */
						while(UART_recieveByte() != UART_START){}

						/*
						 * Door opened successfully
						 */
						LCD_clearScreen();
						LCD_moveCursor(0, 0);
						LCD_displayString("Door Opened");
						_delay_ms(3000);

						/*
						 * Door closing
						 */
						LCD_clearScreen();
						LCD_moveCursor(0, 0);
						LCD_displayString("Door is Closing");
						UART_sendByte(CLOSE_DOOR);

						/*
						 * Waiting until door closes completely
						 */
						while(UART_recieveByte() != UART_START){}

						/*
						 * Door closed successfully
						 */
						LCD_clearScreen();
						LCD_moveCursor(0, 0);
						LCD_displayString("Door Closed");
						_delay_ms(3000);
						UART_sendByte(UART_CONTINUE);
						continue;

					}
				}
				/*
				 * Change password functions
				 */
				else if(KEYPAD_Key_Pressed == '-'){
					/*
					 * Ask user for old password
					 */
					LCD_clearScreen();
					LCD_moveCursor(0, 0);
					LCD_displayString("Enter old");
					LCD_moveCursor(0, 13);
					LCD_displayCharacter('(');
					LCD_intgerToString(incorrect_Passwords+1);
					LCD_displayCharacter(')');
					LCD_moveCursor(1, 0);
					LCD_displayString("Password:");
					LCD_moveCursor(1, 9);

					get_Password(password1);		// Get password
					/*
					 * Send password to CONTROL_ECU to check if it matches
					 */
					UART_sendByte(UART_START);
					UART_sendByte(CHANGE_PASSWORD);
					UART_sendString(password1);

					/*
					 * Wait for response from CONTROL_ECU
					 */
					while(UART_recieveByte() != UART_START){}

					/*
					 * Receive response from CONTROL_ECU
					 */
					UART_received_Byte = UART_recieveByte();

					/*
					 *  If old password was not correct
					 */
					if( UART_received_Byte == INCORRECT_PASSWORD){
						LCD_clearScreen();
						LCD_displayString("Old password");
						LCD_moveCursor(1, 0);
						LCD_displayString("Did not match");
						incorrect_Passwords++;				// Counting incorrect trials
						_delay_ms(1500);
						continue;
					}
					/*
					 * If old password was correct
					 */
					else if(UART_received_Byte == CORRECT_PASSWORD){
						incorrect_Passwords=0;				// Reset incorrect trials
						/*
						 * Ask user for new password
						 */
						LCD_clearScreen();
						LCD_displayString("Enter new");
						LCD_moveCursor(1, 0);
						LCD_displayString("Password:");
						LCD_moveCursor(1, 9);

						get_Password(password1);			// Get password

						/*
						 * Send password to CONTROL_ECU to check
						 */
						UART_sendByte(UART_NEW_PASSWORD_READY);
						UART_sendString(password1);

						/*
						 * Wait for response from CONTROL_ECU
						 */
						while(UART_recieveByte() != UART_START){}
						/*
						 * Receive response from CONTROL_ECU
						 */
						UART_received_Byte = UART_recieveByte();

						/*
						 * If user entered new password matching to the old password
						 */
						if(UART_received_Byte == SAME_PASSWORD){
							LCD_clearScreen();
							LCD_displayString("You entered the");
							LCD_moveCursor(1, 0);
							LCD_displayString("same password");
							_delay_ms(1500);
							LCD_clearScreen();
							LCD_displayString("Password did not");
							LCD_moveCursor(1, 0);
							LCD_displayString("Change");
							_delay_ms(1500);
							continue;
						}
						/*
						 * If user entered new password
						 */
						else if(UART_received_Byte == NEW_PASSWORD){
							LCD_clearScreen();
							LCD_displayString("Password Changed");
							LCD_moveCursor(1, 0);
							LCD_displayString("Successfully");
							_delay_ms(1500);
							/*
							 * Wait for password changing to complete
							 */
							while(UART_recieveByte() != PASSWORD_CHANGED){}
							continue;
						}
					}

				}
				/*
				 * If user selected an incorrect option
				 * ie. something other than Open Door or Change Pass
				 */
				else{
					LCD_clearScreen();
					LCD_moveCursor(0, 0);
					LCD_displayString("Incorrect input");
					_delay_ms(1500);
					continue;
				}

			}
		}
	}
	return 0;
}
