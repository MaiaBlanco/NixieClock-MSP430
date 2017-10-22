//#include <msp430g2553.h>
#include <msp430g2452.h>


#define LED_0 BIT0
#define LED_1 BIT3
#define LED_2 BIT5
#define LED_OUT P1OUT
#define LED_DIR P1DIR
#define BTN_0 0x40
#define BTN_1 0x80


volatile unsigned int timerCount = 0;
volatile unsigned int displayCount = 0;
volatile int countDown = 0;
volatile char button_1_press = 0;
volatile char button_2_press = 0;
volatile unsigned char seconds = 0;
unsigned char minutes;
unsigned char hours;

void update_display();
void draw_digit(char n);


int horner_multiply_60(int a)
{
    // Just for reference:
    // int sixty = 0b111100;
    int x = (a << 2);
    x = x + (a << 3);
    x = x + (a << 4);
    x = x + (a << 5);
    return x;
}

int divide(int a, int d)
{
    // Just for reference:
    // int sixty = 0b111100;
    // 1/60 appears to be a repeating binary number. Going to brute force this.
    int x = 0;
    while (a > 0)
    {
        x++;
        a -= d;
    }
    return x;
}

// not pretty, but should work...
int modulo(int a, int b)
{
    int x = a;
    while (x > b)
        x -= b;
    return x;
}

// void UART_init()
// {
//     /* Configure hardware UART */
//     P1SEL = BIT1 + BIT2; // P1.1 = RXD, P1.2=TXD
//     P1SEL2 = BIT1 + BIT2; // P1.1 = RXD, P1.2=TXD
//     UCA0CTL1 |= UCSSEL_2; // Use SMCLK
//     UCA0BR0 = 0x03; // Set baud rate to 9600 with 32.768KHz clock (Data Sheet 15.3.13)
//     UCA0BR1 = 0x00; // Set baud rate to 9600 with 32.768KHz clock
//     UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
//     UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
//     IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
// }

// void UARTSendArray(unsigned char *TxArray, unsigned char ArrayLength){
//     while(ArrayLength--){ // Loop until StringLength == 0 and post decrement
//         while(!(IFG2 & UCA0TXIFG)); // Wait for TX buffer to be ready for new data
//         UCA0TXBUF = *TxArray; //Write the character at the location specified py the pointer
//         TxArray++; //Increment the TxString pointer to point to the next character
//     }
// }

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    // Clear interrupt flags
    P1IFG = 0x00;
    // Enable interrupts on P1
    P1IE |= (BTN_0 + BTN_1);
    P1IES |= (BTN_0 + BTN_1);

    BCSCTL1 = XTS; //  low frequency mode
    BCSCTL2 = SELS; // Select SMCLK (which can use external crystal) to use LFXT1CLK
    BCSCTL3 = LFXT1S0 + XCAP_3; // Set external 32.768 kHz oscillator crystal w/ 12.5 pf caps
    // Clear the timer and enable timer interrupt
    CCTL0 = CCIE;
    TACCR0 = 0x001F;  // Set capture-compare value to 31 (so that 32 counts occur, including 0)
    TACTL = TASSEL_2 + MC_1; // Set the timer A to SMCLK, Compare Mode to TACCR0

//    UART_init();
//    UARTSendArray("UART UP!\n\r", 10);

    __enable_interrupt();

    //__bis_SR_register(LPM0 + GIE); // LPM0 with interrupts enabled
    __bis_SR_register(GIE); // interrupts enabled

    displayCount = 0;
    while (displayCount < 10000);
    // Set P1 to act as GPIO only (the wait above is important to make sure the MSP can be reprogrammed!)
    P1SEL = 0x00;
    P1SEL2 = 0x00;
    P1DIR |= 0xFF; // Set P1.0 to output direction
    P1OUT &= ~0xFF; // Set the LEDs off
    // Set first 5 bits of P2 to output
    P2DIR |= 0x1F;
    P2OUT &= ~0x1F;
    // Setup 1.6 and 1.7 as interrupt input with no pullup
    P1DIR &= ~(BTN_0 + BTN_1);
    P1REN |= (BTN_0 + BTN_1);

    while (1)
    {
        // Update the display
        if (button_1_press) // Add one minute
        {
            minutes += button_1_press;
            button_1_press = 0;
        }
        if (button_2_press) // Add one hour
        {
            hours += button_2_press;
            button_2_press = 0;
        } 
        if (seconds >= 60)
        {
            minutes += 1;
            seconds -= 60;
        }
        if (minutes >= 60)
        {
            hours += 1;
            minutes -= 60;
        }
        if (hours == 24)
        {
            hours = 0;
        }
        update_display();
    }

}

// Update display tube with current time
// TODO: rewrite to use peripheral hardware multiplier to improve cycle time
void update_display()
{
    // Show hour then minute digits separated by ~0.25 sec
    unsigned char h1 = hours/10;//divide(hour, 10);
    unsigned char h0 = hours%10;//modulo(hour, 10);
    unsigned char m1 = minutes/10;//divide(minute, 10);
    unsigned char m0 = minutes%10;//modulo(minute, 10);
    // UARTSendArray(h1 + '0', 1);
    // UARTSendArray(h0 + '0', 1);
    // UARTSendArray(m1 + '0', 1);
    // UARTSendArray(h0 + '0', 1);
    // UARTSendArray('\n\r', 2);
    draw_digit( h1 );
    displayCount = 0;
    while (displayCount < 128);
    draw_digit( 20 );
    displayCount = 0;
    while (displayCount < 128);
    draw_digit( h0 );
    displayCount = 0;
    while (displayCount < 256);
    draw_digit( 12 );
    displayCount = 0;
    while (displayCount < 128);
    draw_digit( m1 );
    displayCount = 0;
    while (displayCount < 128);
    draw_digit( 20 );
    displayCount = 0;
    while (displayCount < 128);
    draw_digit( m0 );
    displayCount = 0;
    while (displayCount < 128);
    draw_digit( 20 );
    displayCount = 0;
    while (displayCount < 512);
}

void draw_digit(char n)
{
    P1OUT = 0x00;
    P2OUT = 0x00;
    switch (n)
    {
        case 0:
            P1OUT = 0x01;
            break;
        case 9:
            P1OUT = 0x02;
            break;
        case 8:
            P1OUT = 0x04;
            break;
        case 7:
            P1OUT = 0x08;
            break;
        case 6:
            P1OUT = 0x10;
            break;
        case 5:
            P1OUT = 0x20;
            break;
        case 4:
            P2OUT = 0x01;
            break;
        case 3:
            P2OUT = 0x02;
            break;
        case 2:
            P2OUT = 0x04;
            break;
        case 1:
            P2OUT = 0x08;
            break;
        case 10:
            P2OUT = 0x10;
            break;
        case 11:
            P2OUT = 0x20;
            break;
        case 12:
            P2OUT = 0x30;
            break;
        default:
            break;
    }
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    timerCount ++;
    displayCount ++;
    countDown --;
    // When the timer hits 1024 that should be 1 second:
    if(timerCount == 1024)
    {
        timerCount = 0;
        seconds ++;
    }
}


#pragma vector=PORT1_VECTOR
__interrupt void Port1 (void)
{
    P1IE = 0x00;
    if ( countDown <= 0 )
    {
        if (P1IN & BTN_0)
        {
            button_1_press ++;
            //P1OUT ^= LED_1;
        }
        else if (P1IN & BTN_1)
        {
            button_2_press ++;
            //P1OUT ^= LED_2;
        }
        countDown = 250;
    }
    P1IFG = 0x00;
    P1IE = (BTN_0 + BTN_1);
}
