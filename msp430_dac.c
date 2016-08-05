#include <msp430.h>
#include "msp430_dac.h"
/*
 * msp430_dac.c
 *
 *  Created on: Jul 30, 2016
 *      Author: Kyle
 */

void  dac_set(unsigned x){
	char data;
	unsigned output = 0;
	unsigned temp = x;
	temp = temp << 2;
	temp |= 0xF000;
	output = temp;
	data = output >> 8;
	char_set(data);
	data = output;
	char_set(data);
}
void char_set(char x){

  UCA0TXBUF = x;
  while(!(IFG2 & UCA0TXIFG)){
  }
}

void init_spi(){
  P3DIR |= 0x08; 			// setting 3.3 as output for CS
  P3OUT |= 0x08;          //setting 3.3 (CS) high for LTC1661
  P3SEL |= 0x11;                            // P3.0,4 USCI_A0 option select
  UCA0CTL0 |= UCCKPL + UCMSB +UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK set as clock pin
  UCA0BR0 |= 0x02;
  UCA0BR1 = 0;                              //
  UCA0MCTL = 0;                             //not used in SPI mode apparently
  UCA0CTL1 &= ~UCSWRST;                     // software reset of SPI

}
