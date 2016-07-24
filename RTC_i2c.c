#include <msp430.h>
#include "RTC_i2c.h"
#include "LCD.h"

unsigned int alm_flag = 0;

RTC_Time RTC_timedate;


int main(void){

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	init_RTC();
	adc_init();
	//volatile unsigned seconds, minutes, hours = 0;
	lcd_init();
	default_set();
	write_time_RTC(&RTC_timedate);

	while(1){

		read_time_RTC(&RTC_timedate);
		current_time_to_LCD(&RTC_timedate);
		lcd_command(0xC0);
		current_date_to_LCD(&RTC_timedate);
		lcd_command(0x02);
		unsigned int buttons[2];
		buttons[0] = P2IN & 0x02;
		buttons[1] = P2IN & 0x04;
		if(buttons[0] != 0 && alm_flag == 1){
					alm_flag = 0;
		}
		if(buttons[1] != 0 && buttons[0] == 0){ // top button pressed
			set_new_time(&RTC_timedate);
			write_time_RTC(&RTC_timedate);
		}
		if(buttons[0] != 0 && buttons[1] != 0){
			set_new_alarm(&RTC_timedate);
			write_time_ALM0(&RTC_timedate);
			alm_flag = 1;
		}
		P3DIR |= 0x10;
		unsigned p36_temp = P3IN & 0x40;
		unsigned char p3_temp = P3IN & 0x20;

		if(alm_flag == 1 && p3_temp == 0){
			P3OUT ^= 0x10;
			//__delay_cycles(5000);
		}
		else{
			P3OUT &= 0x06;
		}


	}

}



void init_i2c(){
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT	
//	P2DIR |= (1 << 1);
//	P2SEL |= (1 << 1);                        // SMCLK Output
	P3DIR = 0x06;
	P3SEL |= 0x06;                            // Assign I2C pins to USCI_B0
}

void init_RTC(){
	init_i2c();
	write_i2c(CTRL_BYTE, control, 0x00);
	unsigned temp = read_i2c(CTRL_BYTE, rtc_seconds);
	if(!(temp & 0x80)){
		write_i2c(CTRL_BYTE, rtc_seconds, 0x80);
	}
}
/****************************************************************
current_time:
	sets the defalt time to 12 AM

	setting date to 7/15 (due date of module)
/*****************************************************************/
void set_new_time(RTC_Time *definedTime){
	unsigned char done = 1;
	unsigned char temp2, temp;
	unsigned int t_hours, t_minutes, t_seconds;
	while(done){
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	    __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
	   	t_hours = adc_val/44;
	   	t_hours = map_hex_dec(t_hours);
	    definedTime -> hours = t_hours;
	    lcd_command(0x02);
	    temp2 = definedTime -> hours;
	    temp2 = temp2 >> 4;
		lcd_char_push(temp2+0x30);
		temp2 = definedTime -> hours & 0x0F;
		lcd_char_push(temp2+0x30);
	    temp = P2IN & 0x04;
	    if(temp == 0){
	    	done = 0;
	    }
	    else{
	    	done = 1;
	    }
	}
	done = 1;

	while(done){
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		t_minutes = adc_val/17;
		if(t_minutes > 59){
			t_minutes = 59;
		}
		t_minutes = map_hex_dec(t_minutes);
		definedTime -> minutes = t_minutes;
		lcd_command(0x02);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		temp2 = definedTime -> minutes;
		temp2 = temp2 >> 4;
		lcd_char_push(temp2+0x30);
		temp2 = definedTime -> minutes & 0x0F;
		lcd_char_push(temp2+0x30);
		temp = P2IN & 0x04;
		if(temp != 0){
			done = 0;
		}
		else{
			done = 1;
		}
	}
	done = 1;
	while(done){ //weekday
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		t_seconds = adc_val/17;
		if(t_seconds > 59){
			t_seconds = 59;
		}
		t_seconds = map_hex_dec(t_seconds);
		definedTime -> seconds = t_seconds;
		lcd_command(0x02);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		temp2 = definedTime -> seconds;
		temp2 = temp2 >> 4;
		lcd_char_push(temp2+0x30);
		temp2 = definedTime -> seconds & 0x0F;
		lcd_char_push(temp2+0x30);
		temp = P2IN & 0x04;
		if(temp == 0){
			done = 0;
		}
		else{
			done = 1;
		}
	}
	done = 1;
	lcd_command(0x02);
}
void set_new_alarm(RTC_Time *definedTime){
	unsigned char done = 1;
	unsigned char temp2, temp;
	unsigned int t_hours, t_minutes, t_seconds;
	while(done){
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	    __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
	   	t_hours = adc_val/44;
	   	t_hours = map_hex_dec(t_hours);
	    definedTime -> hours = t_hours;
	    lcd_command(0x02);
	    temp2 = definedTime -> hours;
	    temp2 = temp2 >> 4;
		lcd_char_push(temp2+0x30);
		temp2 = definedTime -> hours & 0x0F;
		lcd_char_push(temp2+0x30);
	    temp = P2IN & 0x04;
	    if(temp == 0){
	    	done = 0;
	    }
	    else{
	    	done = 1;
	    }
	}
	done = 1;

	while(done){
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		t_minutes = adc_val/17;
		if(t_minutes > 59){
			t_minutes = 59;
		}
		t_minutes = map_hex_dec(t_minutes);
		definedTime -> minutes = t_minutes;
		lcd_command(0x02);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		temp2 = definedTime -> minutes;
		temp2 = temp2 >> 4;
		lcd_char_push(temp2+0x30);
		temp2 = definedTime -> minutes & 0x0F;
		lcd_char_push(temp2+0x30);
		temp = P2IN & 0x04;
		if(temp != 0){
			done = 0;
		}
		else{
			done = 1;
		}
	}
	done = 1;
	while(done){
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		t_seconds = adc_val/17;
		if(t_seconds > 59){
			t_seconds = 59;
		}
		t_seconds = map_hex_dec(t_seconds);
		definedTime -> seconds = t_seconds;
		lcd_command(0x02);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		lcd_command(0x14);
		temp2 = definedTime -> seconds;
		temp2 = temp2 >> 4;
		lcd_char_push(temp2+0x30);
		temp2 = definedTime -> seconds & 0x0F;
		lcd_char_push(temp2+0x30);
		temp = P2IN & 0x04;
		if(temp == 0){
			done = 0;
		}
		else{
			done = 1;
		}
	}
	done = 1;	
	lcd_command(0x02);
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
unsigned char map_hex_dec(unsigned char val){
	unsigned char temp = (val/10)*6;
	return val+temp;
}

void current_time_to_LCD(RTC_Time *definedTime){
	unsigned char seconds, minutes, hours, temp;
	seconds = definedTime -> seconds;
	minutes = definedTime -> minutes;
	hours 	= definedTime -> hours;
	temp = hours >> 4;
	lcd_char_push(temp+0x30);
	temp = hours & 0x0F;
	lcd_char_push(temp+0x30);
	lcd_char_push(':');
	temp = minutes >> 4;
	lcd_char_push(temp+0x30);
	temp = minutes & 0x0F;
	lcd_char_push(temp+0x30);
	lcd_char_push(':');
	seconds &= 0x7f;
	temp = seconds >> 4;
	lcd_char_push(temp+0x30);
	temp = seconds & 0x0F;
	lcd_char_push(temp+0x30);

}

void current_date_to_LCD(RTC_Time *definedTime){
	unsigned char weekday, date, month, year, temp;
	weekday = definedTime -> weekday;
	//weekday &= 0x07;
	date 	= definedTime -> date;
	month 	= definedTime -> month;
	//month 	&= 0x1F;
	year	= definedTime -> year;
	LCD_weekday_out(weekday);
	temp = date >> 4;
	lcd_char_push(temp+0x30);
	temp = date & 0x0F;
	lcd_char_push(temp+0x30);
	lcd_char_push('-');
	temp = month >> 4;
	lcd_char_push(temp+0x30);
	temp = month & 0x0F;
	lcd_char_push(temp+0x30);
	lcd_char_push('-');
	temp = year >> 4;
	lcd_char_push(temp+0x30);
	temp = year & 0x0F;
	lcd_char_push(temp+0x30);

}
void LCD_weekday_out(unsigned char weekday){
	if(weekday == 1){
		lcd_string_push("U ");
	}
	else if(weekday == 2){
		lcd_string_push("M ");
	}
	else if(weekday == 3){
		lcd_string_push("T ");
	}
	else if(weekday == 4){
		lcd_string_push("W ");
	}
	else if(weekday == 5){
		lcd_string_push("R ");
	}
	else if(weekday == 6){
		lcd_string_push("F ");
	}
	else{
		lcd_string_push("S ");
	}
}
void write_time_ALM0(RTC_Time *definedTime){
	// hours = map_hex_dec(hours);
	// minutes = map_hex_dec(minutes);
	// seconds = map_hex_dec(seconds);

	unsigned char t_weekday = 0x70;
	t_weekday |= definedTime -> weekday;
	write_i2c(CTRL_BYTE, alm0_weekday, definedTime -> weekday);
	write_i2c(CTRL_BYTE, alm0_date, definedTime -> date);
	write_i2c(CTRL_BYTE, alm0_month, definedTime -> month);
	
	write_i2c(CTRL_BYTE, alm0_hours, definedTime -> hours);
	write_i2c(CTRL_BYTE, alm0_minutes, definedTime -> minutes);
	write_i2c(CTRL_BYTE, alm0_seconds, ((definedTime -> seconds)));
	write_i2c(CTRL_BYTE, control, 0x10);
}


void write_time_RTC(RTC_Time *definedTime){
	// hours = map_hex_dec(hours);
	// minutes = map_hex_dec(minutes);
	// seconds = map_hex_dec(seconds);	
	write_i2c(CTRL_BYTE, rtc_weekday, definedTime -> weekday);
	write_i2c(CTRL_BYTE, rtc_date, definedTime -> date);
	write_i2c(CTRL_BYTE, rtc_month, definedTime -> month);
	write_i2c(CTRL_BYTE, rtc_year, definedTime -> year);
	write_i2c(CTRL_BYTE, rtc_hours, definedTime -> hours);
	write_i2c(CTRL_BYTE, rtc_minutes, definedTime -> minutes);
	write_i2c(CTRL_BYTE, rtc_seconds, ((definedTime -> seconds) | 0x80));
}

void read_time_RTC(RTC_Time *definedTime){
	definedTime -> seconds = (read_i2c(CTRL_BYTE, rtc_seconds)) & 0x7f;
	definedTime -> minutes = (read_i2c(CTRL_BYTE, rtc_minutes));
	definedTime -> hours   = (read_i2c(CTRL_BYTE, rtc_hours));
	definedTime -> weekday = (read_i2c(CTRL_BYTE, rtc_weekday)) & 0x07;
	definedTime -> date    = (read_i2c(CTRL_BYTE, rtc_date));
	definedTime -> month   = (read_i2c(CTRL_BYTE, rtc_month)) & 0x1F;
	definedTime -> year    = (read_i2c(CTRL_BYTE, rtc_year));
}
void write_i2c(unsigned char slave_add, unsigned char reg, unsigned char data){
	TX_setup(slave_add);
	tx_data[0] = reg;
	tx_data[1] = data;
	tx_counter = 2;
	ptr_TX = tx_data;
	UCB0CTL1 |= UCTR + UCTXSTT;               // I2C TX, start condition
	__bis_SR_register(CPUOFF+GIE);            // CPU off, enable interrupts	
}
/*//////////////////////////////////////////////////////
	read_i2c
	input: slave address, register
	output: register value
	
	first half of function tranmits register value, then resets ptr



*///////////////////////////////////////////////////////
unsigned char read_i2c(unsigned char slave_add, unsigned char reg){
	TX_setup(slave_add);
	ptr_TX = &reg; //moving register in transmit pointer
	tx_counter = 1;
	reset = 1;
	while (UCB0CTL1 & UCTXSTP);
	UCB0CTL1 |= UCTR + UCTXSTT;
	__bis_SR_register(CPUOFF+GIE);            // CPU off, enable interrupts
	while (UCB0CTL1 & UCTXSTP);
	reset = 0;

	RX_setup(slave_add);
	ptr_RX = &rx_data;
	while (UCB0CTL1 & UCTXSTP); // Ensure stop condition got sent
	UCB0CTL1 |= UCTXSTT; // I2C start condition
	while (UCB0CTL1 & UCTXSTT);             // Start condition sent?
	UCB0CTL1 |= UCTXSTP;                // No Repeated Start: stop condition
	__bis_SR_register(CPUOFF + GIE);
	while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
	return rx_data;

}

void TX_setup(unsigned char address){ //this will be run everytime a transmit will occur
	_DINT();

	RX = 0;
	IE2 &= ~UCB0RXIE; //disable recieve interrupt	
	UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  	UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  	UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
  	UCB0BR1 = 0;
	UCB0I2CSA = address;		//setting slave address
	UCB0CTL1 &= ~UCSWRST;  	//clearing SW reset to resume operation
	IE2 |= UCB0TXIE;  	//enabling TX interrupt
	while (UCB0CTL1 & UCTXSTP);
}

void RX_setup(unsigned char address){
	_DINT();
	RX = 1;
	IE2 &= ~UCB0TXIE;	// Disable TX interrupt
	UCB0CTL1 |= UCSWRST; // Enable SW reset
	UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC; // I2C Master, synchronous mode
	UCB0CTL1 = UCSSEL_2 + UCSWRST; // Use SMCLK, keep SW reset
	UCB0BR0 = 12; // fSCL = SMCLK/12 = ~100kHz
	UCB0BR1 = 0;
	UCB0I2CSA = address; // Slave Address
	UCB0CTL1 &= ~UCSWRST; // Clear SW reset, resume operation
	IE2 |= UCB0RXIE; // Enable RX interrupt
}

#pragma vector = USCIAB0TX_VECTOR
__interrupt void USCIAB0TX_ISR(void){
	if(RX){ //recieving
		*ptr_RX = UCB0RXBUF;
		__bic_SR_register_on_exit(CPUOFF);		
	}
	else{ //transmitting
		if(tx_counter != 0){
			UCB0TXBUF = *ptr_TX++;
			tx_counter--;
		}
		else{
			if(reset){
				reset = 0;
				ptr_TX = tx_data;
				__bic_SR_register_on_exit(CPUOFF);				
			}
			else{

				UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
				IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
				__bic_SR_register_on_exit(CPUOFF);      // Exit LPM0
			}
		}
	}

}

void adc_init(){
	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
	ADC10AE0 |= 0x01;
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
	adc_val = ADC10MEM;
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

