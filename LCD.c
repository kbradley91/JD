/*
 * LCD.c
 *
 *  Created on: Jul 13, 2016
 *      Author: Kyle
 */
#include <msp430.h>
#include "string.h"
#include "lcd.h"
#define delay 25000

/***************************************************************************************
	LCD Header
	USES P4.7:4.0(D7:D0) | P1.1:1.0(P1.1 = Enable, P1.0 = Register Select)
****************************************************************************************/

/***********************************************************************
 *  LCD_commmand
 *  sends out an input byte, X, through port 4
 *
************************************************************************/

void lcd_command(char com)
{
	P4DIR = 0xF0;
	P1DIR = 0x03;
	P1OUT = 0x00;
	P4OUT = 0x00;
	unsigned lstore = com;
	__delay_cycles(delay);

	P1OUT = 0x02;					//set enable
	__delay_cycles(delay);

	P4OUT = lstore;				//send lcd top data
	P1OUT = 0x00;				// E = 0
	__delay_cycles(delay);

	lstore = lstore << 4;
	lstore &= 0xF0;				//keep top bits
	P1OUT = 0x02;					//set enable
	__delay_cycles(delay);

	P4OUT = lstore;				//send lcd both data
	P1OUT = 0x00;					//clear enable
	__delay_cycles(delay);
}

//Series of commands to initilaize the LCD to 8 Bit mode

void lcd_init(void)
{
	lcd_command(0x33);
	lcd_command(0x32);
	lcd_command(0x2C);
	lcd_command(0x0C);
	lcd_command(0x02);
}void lcd_char_push
(char lstore)
{
	P4DIR = 0xF0;
	P1DIR = 0x03;
	P1OUT = 0x01;
	P4OUT = 0x00;
	__delay_cycles(delay);

	P1OUT = 0x03;
	__delay_cycles(delay);


	P4OUT = lstore;				//send lcd data
	P1OUT = 0x01;
	__delay_cycles(delay);

	lstore = lstore << 4;
	lstore &= 0xF0;
	P1OUT = 0x03;
	__delay_cycles(delay);

	P4OUT = lstore;				//send lcd data
	P1OUT = 0x01;
	__delay_cycles(delay);
}//Moves a series of characters to the LCD screen
void lcd_string_push(char* y){
	int i;
	for(i = 0; i < strlen(y); i++){
		lcd_char_push(y[i]);
	}
}

