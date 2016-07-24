/*
	LCD Ohm-meter
	Kyle Bradley
	Junior Design
	MSP430
*/



static unsigned int test1;
static unsigned int average[10];
static unsigned int averVal = 0;
void lcd_string_push(char* y, int size);
void lcd_command(char x);
void lcd_init(void);
void lcd_char_push(char y);

int main(void){
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	lcd_init();
	char test[4] = "test";
	unsigned value = sizeof(test);
 	lcd_string_push(test, value);
	ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled
	ADC10AE0 |= 0x01;
	unsigned count = 0;


	for (;;){
	    ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
	    __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
	    //returns here after sampling complete
	    average[count] = test1;
	    count = count + 1;
	    if(count == 10){
	    	int i;
	    	for(i = 0; i<9; i++){
	    		averVal += average[i];
	    	}

	    	averVal = averVal/10;
	    	count = 0;
	    	printOhms(averVal);
	    }



  }



}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
	test1 = ADC10MEM;
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}



void lcd_command(char x){
	P4DIR = 0xFF; // setting 4 as all outputs P7:4 are being used
	P1DIR = 0x03; // p1.0 and p1.1
	P1OUT = 0x00; //
	P4OUT = 0x00;
	__delay_cycles(22000); //delay for LCD cause its slow
	P4OUT = x;
	P1OUT = 0x02; // setting enable high
	__delay_cycles(22000); //delay for LCD cause its slow
	P4OUT = x;
	P1OUT = 0x00;
	__delay_cycles(22000); //delay for LCD cause its slow
}

void lcd_init(void){

	lcd_command(0x38);
	lcd_command(0x0F);
	lcd_command(0x01);
//	lcd_command(0x0C);
//	lcd_command(0x01);
//	lcd_command(0x00);

}

void lcd_char_push(char y){
	P4DIR = 0xFF;
	P1DIR = 0x03;
	P1OUT = 0x01;
	P4OUT = 0;
	__delay_cycles(22000);
	P1OUT = 3;
	P4OUT = y;
	__delay_cycles(22000);
	P4OUT = y;
	P1OUT = 1;
	__delay_cycles(22000);
}

void lcd_string_push(char* y, int size){
	int i;
	for(i = 0; i < size; i++){
		lcd_char_push(y[i]);
	}
}

float averagefunc(float x, unsigned int y){
	/*needs to take in value and apply function to value
	peicewise easiest implementation */

	if(y < 995){
		//no scaling needed
	}
	else{
		x = x * 1.08;
//		x = x * 1.08;
//		if(y > 1005){
//			x = x * 1.13;
//		}
		if(y == 1006||y == 1005 ||y == 1007 || y == 1008 || y == 1009 || y == 1010 || y == 1011){
			x = x * 1.12;
		}

	}

	//TODO: need to multiply x by 10 to get resistor value, 
	//10K is reference resistor value.
	return x;
}

void printOhms(unsigned int x){
	lcd_command(0x01);
	int testOhms = x;
	int val1 = 1023 - x;
	float y = (float)(10*(float)x/(float)(1023-x));
	y = averagefunc(y, x);
	testOhms = y;
	if(x > 1013){
		lcd_string_push("out of range", 12);
		return;
	}
	else if(x < 89){
		lcd_string_push("out of range", 12);
		return;
	}
//	y = x;
//	y = y/val1;
	int tenpow = 1000;
	while((testOhms/tenpow) == 0){
		tenpow = tenpow/10;
	}


	if(tenpow == 1000){
		y = ((float)testOhms/1000);
		int digit1 = y;
		y = 10*(y - digit1);
		int digit2 = y;
		y = 10*(y-  digit2);
		int digit3 = y;
		y = 10*(y - digit3);
		int digit4 = y;
		lcd_char_push(0x30 + digit1);
		lcd_char_push(0x30 + digit2);
		lcd_char_push(0x30 + digit3);
		lcd_char_push(0x30 + digit4);
		lcd_string_push("K Ohms", 6);
	}
	else if(tenpow ==100){
		y = y/100;

//		y = y/tenpow;
		int digit1 = y;
		y = 10*(y - digit1);
		int digit2 = y;
		y = 10*(y-  digit2);
		int digit3 = y;
		lcd_char_push(0x30 + digit1);
		lcd_char_push(0x30 + digit2);
		lcd_char_push(0x30 + digit3);
		
		lcd_string_push("K Ohms", 6);
	}
	else if(tenpow == 10){
		y = y/10;
		int digit1 = y;
		y = 10*(y - digit1);
		int digit2 = y;
		y = 10*(y - digit2);
		int digit3 = y;
		lcd_char_push(0x30 + digit1);
		lcd_char_push(0x30 + digit2);
		lcd_char_push('.');
		lcd_char_push(0x30 + digit3);
	
		
		lcd_string_push("K Ohms", 6);
	}
	else{
		int digit1 = y;
		y = 10*(y - digit1);
		int digit2 = y;
		y = 10*(y - digit2);
		int digit3 = y;
		y = 10*(y - digit3);
		int digit4 = y;
		lcd_char_push(0x30 + digit1);
		lcd_char_push('.');
		lcd_char_push(0x30 + digit2);
		lcd_char_push(0x30 + digit3);
		lcd_char_push(0x30 + digit4);

		lcd_string_push("K Ohms", 6);


	}



}
