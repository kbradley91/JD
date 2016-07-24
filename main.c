#include <msp430.h> 

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    //WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer

    P2DIR = 0x01;                            // Set P2.0 to output

    while(1){
		P2OUT ^= 0x01;
		int i = 0;
		for(i = 0; i<20000; i++);
    }
}
