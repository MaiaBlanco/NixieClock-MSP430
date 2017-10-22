#include <msp430.h>
//#include "msp430g2553.h"


/**
 * blink.c
 */
void IO_Init();
void Timer0_Init();

volatile unsigned int timerCount = 0;

// Timer Interrupt Handler
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A(void)
{
    timerCount = (timerCount + 1) % 8;
    if (timerCount == 0)
    {
        P1OUT ^= 0x01;
    }
}

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer
	IO_Init();
	Timer0_Init();

//	volatile unsigned int i;		// volatile to prevent optimization

	while(1);
//	{
//		P1OUT ^= 0x01;				// toggle P1.0
//		for(i=10000; i>0; i--);     // delay
//	}
}


void IO_Init()
{
    P1DIR |= 0x01;                  // configure P1.0 as output
    P2DIR &= 0x00;
    P3DIR &= 0x00;
}

void Timer0_Init()
{
    TACCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2;    // Timer to SMCLCK, continuous
    // Clear timer and enable timer interrupt:
    __enable_interrupt();
    __bis_SR_register(LPM0 + GIE); // Low Power Mode 0 w/ Interrupts
}
