#include <msp430.h>

int main(void){

	WDTCTL = WDTPW + WDTHOLD;

	P1DIR |= 0x03; //setting p1.0 and p1.1 to output
	//TODO: need to read in input before loops are set
	while(1){
		unsigned VARIABLE;
		VARIABLE = P2IN & 0x01;
		VARIABLE = VARIABLE & 0x01; //might be redundant but works
		//reading from p2.0, if 1, low freq led output
		if(VARIABLE){
			P1OUT ^= 0x01; //xor'ing p1.0 in order to toggle pin
			int i;
			for(i=0;i<24000;i++); //32768 max value i can set, interesting
			for(i=0;i<20000;i++); //32768 max value i can set, interesting
		}
		else{
			P1OUT ^= 0x02; //xor'ing p1.1 in order to toggle piezo speaker
			int i = 0;
			for(i = 39; i>0;i--); //rough math, didn't make sense
		}
	}
}
