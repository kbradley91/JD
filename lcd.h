/*
 * lcd.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Kyle
 */

#ifndef LCD_H_
#define LCD_H_

void lcd_command(char x);
void lcd_init(void);
void lcd_char_push(char y);
void lcd_string_push(char* y);




#endif /* LCD_H_ */
