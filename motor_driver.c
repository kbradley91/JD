/*
 * motor_driver.c
 *
 *  Created on: Jun 28, 2016
 *      Author: Kyle
 */


#include <msp430.h>
const int clockwise[8] = {0x1,0x3,0x2,0x6,0x4,0xC,0x8,0x9};
const int ccw[8] = {0x9,0x8,0xC,0x4,0x6,0x2,0x3,0x1};
unsigned speed_vect[200];
static unsigned adc_val;
static unsigned temp_speed = 0;
static unsigned current_speed = 0;
static unsigned current_fuel = 100;
static int distance;
static int distance_fuel;
static unsigned fuel;
int do_again = 1;
int ready = 0;
int adc_count;
int i;
int done;
int main(void){

  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  DCOCTL = CALDCO_16MHZ;
  BCSCTL1 = CALBC1_16MHZ;
  P1DIR |= 0x04;
  P4DIR = 0xFF;
  adc_count = 0;
  init_timer();
  init_adc();
  send_right_home_ccw();
  send_left_home_ccw();
  send_init_fuel();
  send_init_speed();
  //send_MPH(14);
  //send_fuel(13);
  //send_MPH(30);
  //send_fuel(14);
  ready = 1;

  while(1){
	 //
	  test_ADC();
  }


}

void init_timer(){
  TACCTL0 = CCIE;                          // CCR0 interrupt enabled
  TACCR0 = 20000;
  TACTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
}

void init_adc(){
  ADC10CTL1 = INCH_2 + CONSEQ_1;
  ADC10AE0 |= 0x10;                         // P2.4 ADC option select
  ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE + MSC; // ADC10ON, interrupt enabled
  ADC10AE0 |= 0x07;
}

void send_init_fuel(){
	int j = 0;
	while(j != 45){
		TACCR0 = 20000;
		for(i = 0; i<7; i++){
			P4OUT = clockwise[i];
			__bis_SR_register(LPM0_bits + GIE);
		}
		j++;
	}
}
void send_init_fuel2(){
	int j = 0;
	while(j != 46){
		TACCR0 = 20000;
		for(i = 0; i < 7; i++){
			P4OUT = ccw[i];
			__bis_SR_register(LPM0_bits + GIE);
		}
		j++;
	}
}
void send_init_speed(){
	int j = 0;
	while(j != 37){
		TACCR0 = 20000;
		for(i = 0; i < 7; i++){
			P4OUT = ccw[i] << 4;
			__bis_SR_register(LPM0_bits + GIE);
		}
		j++;
	}
}
void send_init_speed2(){
	int j = 0;
	while(j != 48){
		TACCR0 = 20000;
		for(i = 0; i<7; i++){
			P4OUT = clockwise[i]<<4;
			__bis_SR_register(LPM0_bits + GIE);
		}
		j++;
	}
}
void send_init_speed3(){
	int j = 0;
	while(j != 45){
		TACCR0 = 20000;
		for(i = 0; i<7; i++){
			P4OUT = ccw[i]<<4;
			__bis_SR_register(LPM0_bits + GIE);
		}
		j++;
	}
}
void test_ADC(){

	TACCR0 = 40250;
	ready = 1;
	if(fuel <= 10){
		P1OUT |= 0x04;
	}
	else{
		P1OUT &= 0xFB;

	}
	__bis_SR_register(LPM0_bits + GIE);	
}
void read_fuel(){
	TACCR0 = 0;
	fuel = adc_val;
	fuel = fuel / 10;
	if(adc_count == 199){
		if(fuel > 100){
			fuel = 100;
		}

		set_fuel(fuel);
	}
	TACCR0 = 40250;
}
void set_fuel(unsigned fuel_new){
	distance_fuel = fuel_new - current_fuel;
	if(distance_fuel < 0){ //fuel is decreasing
		distance_fuel = current_fuel - fuel_new;
		if(distance_fuel > 60){
			send_fuel_through_zero(fuel_new);
		}
		else{
			send_fuel(distance_fuel);
		}
	}
	else{
		if(distance_fuel > 60){
			send_fuel_through_zero(fuel_new);
		}
		else{
			send_fuel_CCW(distance_fuel);
		}
	}
	current_fuel = fuel_new;
}
void read_speed(){
	TACCR0 = 0;
	unsigned speed = adc_val;
	speed_vect[adc_count++] = speed;
	if(adc_count == 200){
		adc_count = 0;		
		unsigned new_speed = get_speed();
		//check_speed(new_speed);
		if(temp_speed == 2){
			temp_speed = 0;
			set_speed(new_speed);
		}
		else{
			temp_speed++;
		}

		
	}
	TACCR0 = 40250;
}
void check_speed(unsigned new_speed){
	if(new_speed - temp_speed >= 2){
		do_again = 1;
		temp_speed = new_speed;
	}
	else if(temp_speed - new_speed >= 2){
		do_again = 1;
		temp_speed = new_speed;

	}
	else{
		temp_speed = new_speed;
		do_again = 0;
	}


}
void set_speed(unsigned newSpeed){
	distance = newSpeed - current_speed;
	if(distance < 0){ //speed is decreasing
		distance = current_speed - newSpeed;
		if(distance > 60){
			send_speed_through_zero(newSpeed);
		}
		else{
			send_MPH_CW(distance);
		}
	}
	else{
		if(distance > 60){
			send_speed_through_zero(newSpeed);
		}
		else{
			send_MPH(distance);
		}
	}
	current_speed = newSpeed;
}
void send_fuel_through_zero(unsigned fuel){
	if(current_fuel > 50){
		send_right_home_cw();
		send_init_fuel2();
		distance_fuel = fuel;
		send_fuel_CCW(distance_fuel);
	}
	else{
		send_right_home_ccw();
		send_init_fuel();
		distance_fuel = 100 - fuel;
		send_fuel(distance_fuel);
	}
}
void send_speed_through_zero(unsigned speed){
	if(current_speed > 50){		//send to sensor clockwise
		//need to send home clockwise
		send_left_home_cw();
		send_init_speed3();
		distance = speed;
		send_MPH(distance);
	}
	else{		//send to sensor counter-clockwise
		send_left_home_ccw();
		send_init_speed2(); // sends sensor to 100
		distance = 100 - speed;
		send_MPH_CW(distance);
	}
}
void send_MPH_CW(int speed){
	if(speed > 50){
		speed = speed + 4;
	}
	else if(speed >= 40){
		speed = speed + 3;
	}
	else if(speed >= 20){
		speed = speed + 2;
	}
	else if(speed != 0){
		speed++;
	}
	while(speed != 0){
		int j = 0;
		while(j != 4){
			TACCR0 = 20000;
			for(i = 0; i < 7; i++){
				P4OUT = clockwise[i] << 4;
				__bis_SR_register(LPM0_bits + GIE);
			}
			j++;
		}
		speed--;
	}
}
unsigned get_speed(){
	unsigned high_low = 0;
	for(i = 0; i<200; i++){
		unsigned temp = speed_vect[i];
		if(temp > 0x1FF){
			if(speed_vect[i+1] < 0x1FF & i != 199){
				high_low++;
			}
		}
		else{
			if(speed_vect[i+1] > 0x1FF & i != 199){
				high_low++;
			}
		}
	}
	if(high_low > 100){
		high_low = 100;
	}
	return high_low;
}
void send_fuel(int fuel){
	if(fuel > 40){
		fuel = fuel + 3;
	}
	else if(fuel >= 15){
		fuel = fuel + 2;
	}
	else if(fuel >= 10){
		fuel++;
	}
	while(fuel != 0){
		int j = 0;
		while(j != 4){
			TACCR0 = 20000;
			for(i = 0; i < 7; i++){
				P4OUT = clockwise[i];
				__bis_SR_register(LPM0_bits + GIE);
			}
			j++;
		}
		fuel--;
	}
}
void send_fuel_CCW(int fuel){
	if(fuel > 40){
		fuel = fuel + 3;
	}
	else if(fuel >= 15){
		fuel = fuel + 2;
	}
	else if(fuel >= 10){
		fuel++;
	}
	while(fuel != 0){
		int j = 0;
		while(j != 4){
			TACCR0 = 20000;
			for(i = 0; i < 7; i++){
				P4OUT = ccw[i];
				__bis_SR_register(LPM0_bits + GIE);
			}
			j++;
		}
		fuel--;
	}
}
void send_MPH(int speed){
	if(speed > 50){
		speed = speed + 4;
	}
	else if(speed >= 40){
		speed = speed + 3;
	}
	else if(speed >= 20){
		speed = speed + 2;
	}
	else if(speed != 0){
		speed++;
	}
	while(speed != 0){
		int j = 0;
		while(j != 4){
			TACCR0 = 20000;
			for(i = 0; i < 7; i++){
				P4OUT = ccw[i] << 4;
				__bis_SR_register(LPM0_bits + GIE);
			}
			j++;
		}
		speed--;
	}
}
void send_right_home_ccw(){
	done = 0;
	while(!done){
		int temp = P1IN;
		temp &= 0x01; //P1.0 is right
	//	temp &= 0x02; //P1.1 is left
		if(temp){
			TACCR0 = 0;
			done = 1;


		}
		else{

			TACCR0 = 20000;
			for(i = 0; i<7; i++){
				P4OUT = clockwise[i];
				__bis_SR_register(LPM0_bits + GIE);
			}
		}
	}
}
void send_right_home_cw(){
	done = 0;
	while(!done){
		int temp = P1IN;
		temp &= 0x01; //P1.0 is right
	//	temp &= 0x02; //P1.1 is left
		if(temp){
			TACCR0 = 0;
			done = 1;


		}
		else{

			TACCR0 = 20000;
			for(i = 0; i<7; i++){
				P4OUT = ccw[i];
				__bis_SR_register(LPM0_bits + GIE);
			}
		}
	}
}
void send_left_home_ccw(){
	done = 0;
	while(!done){
		int temp = P1IN;
	//	temp &= 0x01; //P1.0 is right
		temp &= 0x02; //P1.1 is left
		if(temp){
			TACCR0 = 0;
			done = 1;

		}
		else{

			TACCR0 = 20000;
			for(i = 0; i<7; i++){
				P4OUT = clockwise[i]<<4;
				__bis_SR_register(LPM0_bits + GIE);
			}
		}
	}
}
void send_left_home_cw(){
	done = 0;
	while(!done){
		int temp = P1IN;
	//	temp &= 0x01; //P1.0 is right
		temp &= 0x02; //P1.1 is left
		if(temp){
			TACCR0 = 0;
			done = 1;

		}
		else{

			TACCR0 = 20000;
			for(i = 0; i<7; i++){
				P4OUT = ccw[i]<<4;
				__bis_SR_register(LPM0_bits + GIE);
			}
		}
	}
}
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
	  adc_val = ADC10MEM;
	  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}
#pragma vector=TIMERA0_VECTOR
__interrupt void TIMERA0_ISR(void){
	if(ready == 1){
		ready = 0;
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
		__bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
		read_speed();
		read_fuel();
		//P1OUT ^= 0x04;
		ready = 1;
	}
	__bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

