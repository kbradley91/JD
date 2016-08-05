#include <msp430.h> 
#include "RTC_i2c.h"
#include "lcd.h"
#include "msp430_dac.h"
/*
 * main.c
 */


static RTC_Time RTC_timedate;
static RTC_Time AlarmAccessed;
RTC_Time menu_alarm;

unsigned int test_adc = 1;

unsigned int light_sensor, number_val, adc_val;
unsigned sin_index = 0;
unsigned alarm_on = 1;
unsigned alarm_set = 0;

 int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    DCOCTL = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;
//    lcd_init();
//    lcd_command(0x01);
    init_RTC();
	default_set();
	write_time_RTC(&RTC_timedate);
    init_spi();
    init_adc();
    P1DIR |= 0x04;
    P2SEL &= 0xBF;
//    init_timer();



    unsigned char p_button, dt_switch;
    while(1){

    	p_button = !(P2IN & 0x04);
    	dt_switch = (P2IN & 0x40);
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force ex


		P1OUT ^= 0x04;
		int alarm_tripped = check_alarm();
		unsigned char test_lcd;
		if(alarm_tripped && alarm_set){
			lcd_command(0x01);
			lcd_string_push("alarm on");

			while(alarm_on){
				p_button = !(P2IN & 0x04);
				sin_wave();
				if(p_button){
					alarm_on = 0;
				}
			}
			alarm_on = 1;
			lcd_command(0x02);
		}

//		sin_wave();
		if(p_button){
			lcd_command(0x01);
			write_time_RTC(&RTC_timedate);
			lcd_string_push("too true");

			read_time_RTC(&AlarmAccessed);
			lcd_string_push("what can you do");
//			AlarmAccessed -> seconds = AlarmAccessed -> seconds - 0x30;
			write_time_ALM0(&AlarmAccessed);
			lcd_string_push("all these things are");
			unsigned char asdf = read_i2c(CTRL_BYTE, alm0_weekday);
			lcd_string_push("falling away from you");
			write_i2c(CTRL_BYTE, alm0_weekday, asdf);


			lcd_command(0x02);
			menu1(&AlarmAccessed);
//			sin_wave();
		}
//		else if(dt_switch){
//			lcd_command(0xC0);
//			read_time_RTC(&RTC_timedate);
//			current_time_to_LCD(&RTC_timedate);
//			lcd_command(0x02);
//		}
		//		else{
//			lcd_command(0x01);
//			lcd_command(0x02);
//		}

//


    }
}

int check_time(RTC_Time *clockTime, RTC_Time *previousTime){
	if(clockTime->seconds != previousTime -> seconds){
		return 1;
	}
	else if(clockTime -> minutes != previousTime -> minutes){
		return 1;
	}
	else if(clockTime -> hours != previousTime -> hours){
		return 1;
	}
	else{
		return 0;
	}

}
unsigned char temp_function(RTC_Time * defaultTime){
	return defaultTime -> seconds;
}
void default_set(){
	RTC_timedate.seconds = seconds_default;
	RTC_timedate.minutes = minutes_default;
	RTC_timedate.hours 	= hours_default;
	RTC_timedate.weekday = weekday_default;
	RTC_timedate.date   = date_default;
	RTC_timedate.month = month_default;
	RTC_timedate.year = year_default;
}
void sin_wave(){
  P3OUT &= 0xF7;
  dac_set(sin[sin_index++]);
  P3OUT |= 0x08;
  if(sin_index == 64){
    sin_index = 0;
  }
}

void menu1(){
	lcd_command(0x01);
//	P3DIR &= DF;

	char p_button = !(P2IN & 0x04);
	char dt_switch = (P2IN & 0x40);


	while(p_button){
		p_button = !(P2IN & 0x04);
		if(!p_button){
			break;
		}
		else{
			lcd_command(0x02);
			lcd_string_push("stuck");
		}
	}
	lcd_command(0x01);
	lcd_command(0x02);
	lcd_string_push("state1 options");
	lcd_command(0xC0);
	lcd_string_push("alarm: on --- off");
	while(!p_button){
		p_button = !(P2IN & 0x04);
		dt_switch = (P2IN & 0x40);
		char mfp = (P3IN & 0x20);
		if(p_button){
			break;
		}
		if(mfp == 0){
			break;
		}
	}
	if(dt_switch){
		alarm_set = 1;
	}
	else{
		alarm_set = 0;
	}

	lcd_command(0x01);
	while(p_button){
			p_button = !(P2IN & 0x04);
			if(!p_button){
				break;
			}

			else{
				lcd_command(0x02);
				lcd_string_push("stuck3");
			}
	}
	if(alarm_set){
		lcd_command(0x02);
		lcd_string_push("awaiting alarm");
	}
	else{
		lcd_command(0x02);
		lcd_string_push("alarm not set");
	}
//	lcd_command(0x01);
//	while(!p_button){
//		p_button = !(P2IN & 0x04);
//		lcd_command(0x02);
//		lcd_string_push("state2 options");
//		lcd_command(0xC0)
//		lcd_string_push("last alarm  next menu")
//		if(p_button){
//			break;
//		}
//	}
//	lcd_command(0x01);

}
void init_adc(){
    ADC10CTL1 = INCH_2 + CONSEQ_1;
    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE + MSC; // ADC10ON, interrupt enabled
    ADC10AE0 |= 0x03;
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
	adc_val = ADC10MEM;
	if(test_adc == 1){
		light_sensor = adc_val;
		test_adc = 0;
	}
	else{
		test_adc = 1;
		number_val = adc_val;

	}
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}
//void init_timer(){
//  TACCTL0 = CCIE;                          // CCR0 interrupt enabled
//  TACCR0 = 2000;
//  TACTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
//}
//#pragma vector=TIMERA0_VECTOR
//__interrupt void TIMERA0_ISR(void){

////	sin_wave();
//	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
//
//}

int check_alarm(){ //this function is used to see if light is shining on photo-resistor
	if(light_sensor < 700){
		return 1;
	}
	else{
		return 0;
	}

}

