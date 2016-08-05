/*
 * lcd.h
 *
 *  Created on: Jul 13, 2016
 *      Author: Kyle
 */

#ifndef LCD_H_
#define LCD_H_

typedef struct{
	unsigned char* firstLine;
	unsigned char* secondLine;
	unsigned char update_flag;

}LCD_display;

void set_new_strings(LCD_display *screen, char* line_1, char* line_2);
void update_LCD(LCD_display *userLCD);

void lcd_command(char x);
void lcd_init(void);
void lcd_char_push(char y);
void lcd_string_push(char* y);


#endif /* LCD_H_ */
