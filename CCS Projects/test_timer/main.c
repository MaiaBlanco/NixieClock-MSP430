//***************************************************************************************
// MSP430 Timer Blink LED Demo - Timer A Software Toggle P1.0 & P1.6
//
// Description; Toggle P1.0 and P1.6 by xor'ing them inside of a software loop.
// Since the clock is running at 1Mhz, an overflow counter will count to 8 and then toggle
// the LED. This way the LED toggles every 0.5s.
// ACLK = n/a, MCLK = SMCLK = default DCO
//
// MSP430G2xx
// -----------------
// /|\| XIN|-
// | | |
// --|RST XOUT|-
// | P1.6|-->LED
// | P1.0|-->LED
//
// Aldo Briano
// Texas Instruments, Inc
// June 2010
// Built with Code Composer Studio v4
//***************************************************************************************
#include <msp430g2553.h>


#define LED_0 BIT0
#define LED_1 BIT6
#define LED_OUT P1OUT
#define LED_DIR P1DIR



unsigned int timerCount = 0;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    LED_DIR |= (LED_0 + LED_1); // Set P1.0 and P1.6 to output direction
    LED_OUT &= ~(LED_0 + LED_1); // Set the LEDs off

    CCTL0 = CCIE;
    TACTL = TASSEL_2 + MC_2; // Set the timer A to SMCLCK, Continuous
    // Clear the timer and enable timer interrupt

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 with interrupts enabled

}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    timerCount ++;
    // Modulo 8:
    timerCount = (timerCount - ((timerCount >> 3) << 3));
    if(timerCount == 0) P1OUT ^= (LED_0 + LED_1);
}
