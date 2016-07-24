/*
 * RTC_i2c.h
 *
 *  Created on: Jul 12, 2016
 *      Author: Kyle
 */

#ifndef RTC_I2C_H_
#define RTC_I2C_H_

//bit definitions

#define CTRL_BYTE 0x6F

//counter registers

#define rtc_seconds 0x00
#define rtc_minutes 0x01
#define rtc_hours	0x02
#define rtc_weekday	0x03
#define rtc_date	0x04
#define rtc_month	0x05
#define rtc_year 	0x06
#define control		0x07
#define osc_trim	0x08


//alarm0 registers

#define alm0_seconds 	0x0A
#define alm0_minutes 	0x0B
#define alm0_hours	0x0C
#define alm0_weekday	0x0D
#define alm0_date	0x0E
#define alm0_month	0x0F

//default date values
#define weekday_default 0x04
#define date_default	0x13
#define month_default 	0x07
#define year_default 	0x16
#define hours_default 	0x12
#define minutes_default 0x59
#define seconds_default 0x00


unsigned char		RX, reset; //reset flag
unsigned char 	tx_data[2];
unsigned char		tx_counter;
unsigned char 	rx_data;
unsigned char 	*ptr_TX, *ptr_RX;

unsigned int adc_val;

typedef struct{
	unsigned char seconds;
	unsigned char minutes;
	unsigned char hours;
	unsigned char weekday;
	unsigned char date;
	unsigned char month;
	unsigned char year;
}RTC_Time;



#endif /* RTC_I2C_H_ */
