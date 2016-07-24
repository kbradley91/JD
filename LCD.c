/*
 * LCD.c
 *
 *  Created on: Jul 13, 2016
 *      Author: Kyle
 */
#include <msp430.h>
#include "string.h"
#include "lcd.h"
/***************************************************************************************
	LCD Header
	USES P4.7:4.0(D7:D0) | P1.1:1.0(P1.1 = Enable, P1.0 = Register Select)
****************************************************************************************/

/***********************************************************************
 *  LCD_commmand
 *  sends out an input byte, X, through port 4
 *
************************************************************************/
void lcd_command(char x){
	P4DIR = 0xFF; // setting 4 as all outputs P7:4 are being used
	P1DIR = 0x03; // p1.0 and p1.1
	P1OUT = 0x00; //
	P4OUT = 0x00;
	__delay_cycles(150); //delay for LCD cause its slow
	P4OUT = x;
	P1OUT = 0x02; // setting enable high
	__delay_cycles(150); //delay for LCD cause its slow
	P4OUT = x;
	P1OUT = 0x00;
	__delay_cycles(150); //delay for LCD cause its slow
}


//Series of commands to initilaize the LCD to 8 Bit mode
void lcd_init(void){

	lcd_command(0x38);
	lcd_command(0x0F);
	lcd_command(0x01);
//	lcd_command(0x0C);
//	lcd_command(0x01);
//	lcd_command(0x00);

}
//Moves a character to the LCD screen
void lcd_char_push(char y){
	P4DIR = 0xFF;
	P1DIR = 0x03;
	P1OUT = 0x01;
	P4OUT = 0;
	__delay_cycles(150);
	P1OUT = 3;
	P4OUT = y;
	__delay_cycles(150);
	P4OUT = y;
	P1OUT = 1;
	__delay_cycles(150);
}
//Moves a series of characters to the LCD screen
void lcd_string_push(char* y){
	int i;
	for(i = 0; i < strlen(y); i++){
		lcd_char_push(y[i]);
	}
}

