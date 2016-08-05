#include <msp430.h>

// TODO: NEED TO CHANGE CHIPSELECT PIN IN DAC FUNCTIONS
// CURRENTLY HOOKED UP TO 3.3 in the schematic

static unsigned adc_val;

unsigned sin[64] = {0x200,0x232,0x263,0x294,0x2c3,0x2f1,0x31c,0x344,
		0x369,0x38b,0x3a9,0x3c3,0x3d8,0x3e9,0x3f5,0x3fd,
		0x3ff,0x3fd,0x3f5,0x3e9,0x3d8,0x3c3,0x3a9,0x38b,
		0x369,0x344,0x31c,0x2f1,0x2c3,0x294,0x263,0x232,
		0x200,0x1cd,0x19c,0x16b,0x13c,0x10e,0xe3,0xbb,
		0x96,0x74,0x56,0x3c,0x27,0x16,0xa,0x2,
		0x0,0x2,0xa,0x16,0x27,0x3c,0x56,0x74,
		0x96,0xbb,0xe3,0x10e,0x13c,0x16b,0x19c,0x1cd};
unsigned triangle[64] = {0x20,0x40,0x60,0x80,0xa0,0xc0,0xe0,0x100,
		0x120,0x140,0x160,0x180,0x1a0,0x1c0,0x1e0,0x200,
		0x21f,0x23f,0x25f,0x27f,0x29f,0x2bf,0x2df,0x2ff,
		0x31f,0x33f,0x35f,0x37f,0x39f,0x3bf,0x3df,0x3ff,
		0x3df,0x3bf,0x39f,0x37f,0x35f,0x33f,0x31f,0x2ff,
		0x2df,0x2bf,0x29f,0x27f,0x25f,0x23f,0x21f,0x200,
		0x1e0,0x1c0,0x1a0,0x180,0x160,0x140,0x120,0x100,
		0xe0,0xc0,0xa0,0x80,0x60,0x40,0x20,0x0};
unsigned sawtooth[64] = {
		0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
		0x90,0xa0,0xb0,0xc0,0xd0,0xe0,0xf0,0x100,
		0x110,0x120,0x130,0x140,0x150,0x160,0x170,0x180,
		0x190,0x1a0,0x1b0,0x1c0,0x1d0,0x1e0,0x1f0,0x200,
		0x20f,0x21f,0x22f,0x23f,0x24f,0x25f,0x26f,0x27f,
		0x28f,0x29f,0x2af,0x2bf,0x2cf,0x2df,0x2ef,0x2ff,
		0x30f,0x31f,0x32f,0x33f,0x34f,0x35f,0x36f,0x37f,
		0x38f,0x39f,0x3af,0x3bf,0x3cf,0x3df,0x3ef,0x3ff
};
unsigned square[64] = {
		0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,
		0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,
		0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,
		0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,0x3FF,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0


};
unsigned i;
int main(void){

  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  DCOCTL = CALDCO_16MHZ;
  BCSCTL1 = CALBC1_16MHZ;
  init_spi();
  init_adc();
  init_timer();

  while(1){
	  __bis_SR_register(LPM0_bits + GIE);
  }
}
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
  P3DIR |= 0x02; //
  P3OUT |= 0x02;          //setting 3.1 (CS) high for LTC1661
  P3SEL |= 0x11;                            // P3.0,4 USCI_A0 option select


  UCA0CTL0 |= UCCKPL + UCMSB +UCMST + UCSYNC;  // 3-pin, 8-bit SPI master
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK set as clock pin
  UCA0BR0 |= 0x02;
  UCA0BR1 = 0;                              //
  UCA0MCTL = 0;                             //not used in SPI mode apparently
  UCA0CTL1 &= ~UCSWRST;                     // software reset of SPI

}
void sin_wave(){
  unsigned temp = adc_val;
  if(adc_val < 204){
    adc_val = 204;
  }
  float temp2 = sin[i++];  
  temp2 = temp2*adc_val/(float)0x3FF;
  P3OUT &= 0xFD;
  dac_set((unsigned)temp2);
  P3OUT |= 0x02;
  if(i == 64){
    i = 0;
  }


}
void sawtooth_wave(){
	unsigned temp = adc_val;
	  if(adc_val < 204){
	    adc_val = 204;
	  }
	  float temp2 = sawtooth[i++];
	  temp2 = temp2*adc_val/(float)0x3FF;
	  P3OUT &= 0xFD;
	  dac_set((unsigned)temp2);
	  P3OUT |= 0x02;
	  if(i == 64){
	    i = 0;
	  }
}
void triangle_wave(){
	unsigned temp = adc_val;
	  if(adc_val < 204){
	    adc_val = 204;
	  }
	  float temp2 = triangle[i++];
	  temp2 = temp2*adc_val/(float)0x3FF;
	  P3OUT &= 0xFD;
	  dac_set((unsigned)temp2);
	  P3OUT |= 0x02;
	  if(i == 64){
	    i = 0;
	  }
}
void square_wave(){
	unsigned temp = adc_val;
	  if(adc_val < 204){
	    adc_val = 204;
	  }
	  float temp2 = square[i++];
	  temp2 = temp2*adc_val/(float)0x3FF;
	  P3OUT &= 0xFD;
	  dac_set((unsigned)temp2);
	  P3OUT |= 0x02;
	  if(i == 64){
	    i = 0;
	  }
}

void init_adc(){
  ADC10CTL1 = INCH_2 + CONSEQ_1;
  ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE + MSC; // ADC10ON, interrupt enabled
  ADC10AE0 |= 0x07;
}
void init_timer(){
  TACCTL0 = CCIE;                          // CCR0 interrupt enabled
  TACCR0 = 25000;
  TACTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR
}

void set_Freq(){
  if(adc_val < 102){
    adc_val = 102;
  }
  unsigned freq = adc_val;
  float freq_temp = 25000;
  freq_temp = freq_temp*(float)102/freq;
  TACCR0 = (unsigned)freq_temp;
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void){
  adc_val = ADC10MEM;
  __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}
#pragma vector=TIMERA0_VECTOR
__interrupt void TIMERA0_ISR(void){

  ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
  __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
//  init_adc2();
  ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
  __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
  set_Freq();
  //sin_wave();
  //sawtooth_wave();
  //triangle_wave();
  unsigned buttons = P1IN;
  buttons &= 0x03;
  if(buttons == 0){
    sin_wave();
  }
  else if(buttons == 1){
    sawtooth_wave();
  }
  else if(buttons == 2){
    triangle_wave();
  }
  else{
    square_wave();
  }
}
