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
    TACCR0 = 0x1FFF;  // Set capture-compare value to
    TACTL = TASSEL_2 + MC_1; // Set the timer A to SMCLK, Compare Mode to TACCR0
    BCSCTL1 = XTS; //  low frequency mode
    BCSCTL2 = SELS; // Select SMCLK (which can use external crystal) to use LFXT1CLK
    BCSCTL3 = LFXT1S0 + XCAP_3; // Set external 32.768 kHz oscillator crystal w/ 12.5 pf caps
    // Clear the timer and enable timer interrupt

    __enable_interrupt();

    __bis_SR_register(LPM0 + GIE); // LPM0 with interrupts enabled

}


// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
//    timerCount ++;
//    // Modulo 8:
//    timerCount = (timerCount - ((timerCount >> 3) << 3));
//    if(timerCount == 0)
    P1OUT ^= (LED_0 + LED_1);
}
