 /******************************************************************************
 *
 * File Name: UART.h
 *
 * Description: Header file for the UART driver
 *
 * Author: Ahmed Hany
 *
 *******************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "../std_types.h"




extern uint8 password[5];
extern uint8 rec_byte;


/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	Parity_OFF,EVEN=2,ODD
}UART_parity_Bit;

typedef enum
{
	Single,Double
}UART_transmission_Speed;

typedef enum
{
	UART_INT_OFF,UART_INT_ON
}UART_receive_Interrupt;

typedef enum
{
	One_bit,Two_bits
}UART_stop_Bit;

typedef struct
{
	uint32 baud_rate;
	UART_parity_Bit parity;
	UART_transmission_Speed speed;
	UART_receive_Interrupt rx_interrupt;
	UART_stop_Bit stop;
}UART_ConfigType;
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Functional responsible for Initialize the UART device by:
 * 1. Setup the Frame format like number of data bits, parity bit type and number of stop bits.
 * 2. Enable the UART.
 * 3. Setup the UART baud rate.
 */
void UART_init(const UART_ConfigType *Config_Ptr);

/*
 * Description :
 * Functional responsible for send byte to another UART device.
 */
void UART_sendByte(const uint8 data);

/*
 * Description :
 * Functional responsible for receive byte from another UART device.
 */
uint8 UART_recieveByte(void);

/*
 * Description :
 * Send the required string through UART to the other UART device.
 */
void UART_sendString(const uint8 *Str);

/*
 * Description :
 * Receive the required string until the '#' symbol through UART from the other UART device.
 */
void UART_receiveString(uint8 *Str); // Receive until #

#endif /* UART_H_ */
