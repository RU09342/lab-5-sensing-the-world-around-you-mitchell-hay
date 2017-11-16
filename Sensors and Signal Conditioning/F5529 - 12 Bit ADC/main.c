/*
 * Mitchell Hay, Shani Thapa
 * RU 09342-4
 * Analog to Digital Converter - 12 Bit
 */

//                MSP430F552x
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//     Vin -->|P6.0/CB0/A0  P1.0|--> LED
//
//   Bhargavi Nisarga
//   Texas Instruments Inc.
//   April 2009
//   Built with CCSv4 and IAR Embedded Workbench Version: 4.21
//******************************************************************************

#include <msp430f5529.h>

volatile unsigned int val = 0;

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
	ADC12CTL1 = ADC12SHP;                     // Use sampling timer
	ADC12IE = 0x01;                           // Enable interrupt
	ADC12CTL0 |= ADC12ENC;

	P6SEL |= 0x01;                            // P6.0 ADC option select
	P1DIR |= 0x01;                            // P1.0 output

	TA0CCTL0 = CCIE;                             // CCR0 interrupt enabled
	TA0CTL = TASSEL_1 + MC_1 + ID_3;           // ACLK/8, upmode
	TA0CCR0 = 4000;                           // 1 Hz

	// UART Setup
	P3SEL |= BIT3 + BIT4;                       // P3.3,4 = USCI_A0 TXD/RXD
	UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
	UCA0CTL1 |= UCSSEL_1;                     	// ACLK
	UCA0BR0 = 3;                        // 32726 MHz/3 = 9600 (see User's Guide)
	UCA0BR1 = 0;                              	// 1MHz 3
	UCA0MCTL |= UCBRS_3 + UCBRF_0;            	// Modulation UCBRSx=1, UCBRFx=0
	UCA0CTL1 &= ~UCSWRST;                   // **Initialize USCI state machine**

	while (1) {
		ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion

		__bis_SR_register(GIE);     // LPM0, ADC12_ISR will force exit
		__no_operation();                       // For debugger
	}
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch (__even_in_range(ADC12IV, 34)) {
	case 0:
		break;                           // Vector  0:  No interrupt
	case 2:
		break;                           // Vector  2:  ADC overflow
	case 4:
		break;                           // Vector  4:  ADC timing overflow
	case 6:                                  // Vector  6:  ADC12IFG0
		if (ADC12MEM0 >= 0x7ff)                 // ADC12MEM = A0 > 0.5AVcc?
				{
			//P1OUT |= BIT0;                        // P1.0 = 1
			P1OUT &= ~BIT0;
			val = ADC12MEM0;
		} else {
			//P1OUT &= ~BIT0;                       // P1.0 = 0
			P1OUT |= BIT0;
			val = ADC12MEM0;
		}

		__bic_SR_register_on_exit(LPM0_bits);   // Exit active CPU
	case 8:
		break;                           // Vector  8:  ADC12IFG1
	case 10:
		break;                           // Vector 10:  ADC12IFG2
	case 12:
		break;                           // Vector 12:  ADC12IFG3
	case 14:
		break;                           // Vector 14:  ADC12IFG4
	case 16:
		break;                           // Vector 16:  ADC12IFG5
	case 18:
		break;                           // Vector 18:  ADC12IFG6
	case 20:
		break;                           // Vector 20:  ADC12IFG7
	case 22:
		break;                           // Vector 22:  ADC12IFG8
	case 24:
		break;                           // Vector 24:  ADC12IFG9
	case 26:
		break;                           // Vector 26:  ADC12IFG10
	case 28:
		break;                           // Vector 28:  ADC12IFG11
	case 30:
		break;                           // Vector 30:  ADC12IFG12
	case 32:
		break;                           // Vector 32:  ADC12IFG13
	case 34:
		break;                           // Vector 34:  ADC12IFG14
	default:
		break;
	}
}

// Timer A0 Interrupt Service Routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
	while (!(UCA0IFG & UCTXIFG));
	UCA0TXBUF = val;
}
