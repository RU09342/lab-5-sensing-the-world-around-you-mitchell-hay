#include <msp430.h>
unsigned int ADC_Result;

void Init_GPIO();							// Initialize GPIO Pins
void Software_Trim();            			// Software Trim to get the best DCOFTRIM value
#define MCLK_FREQ_MHZ 8                     // MCLK = 8MHz

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;                // Stop watchdog timer

	// Configure GPIO
	Init_GPIO();

	PM5CTL0 &= ~LOCKLPM5; 						// Disable the GPIO power-on default high-impedance mode
						  	  	  	  	  	  	// to activate 1previously configured port settings

	__bis_SR_register(SCG0);                 	// disable FLL
	CSCTL3 |= SELREF__REFOCLK;               	// Set REFO as FLL reference source
	CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3; // DCOFTRIM=3, DCO Range = 8MHz
	CSCTL2 = FLLD_0 + 243;                  	// DCODIV = 8MHz
	__delay_cycles(3);
	__bic_SR_register(SCG0);                	// enable FLL
	Software_Trim();             				// Software Trim to get the best DCOFTRIM value

	CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;	// set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
											   	// default DCODIV as MCLK and SMCLK source

	// Configure UART pins
	P1SEL0 |= BIT6 | BIT7;                  	// set 2-UART pin as second function

	// Configure UART
	UCA0CTLW0 |= UCSWRST;
	UCA0CTLW0 |= UCSSEL__SMCLK;

	// Baud Rate calculation
	// 8000000/(16*9600) = 52.083
	// Fractional portion = 0.083
	// User's Guide Table 17-4: UCBRSx = 0x49
	// UCBRFx = int ( (52.083-52)*16) = 1
	UCA0BR0 = 52;                             // 8000000/16/9600
	UCA0BR1 = 0x00;
	UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

	UCA0CTLW0 &= ~UCSWRST;                    		// Initialize eUSCI

	// Configure ADC10
	ADCCTL0 |= ADCSHT_2 | ADCON;                    // ADCON, S&H=16 ADC clks
	ADCCTL1 |= ADCSHP;                        		// ADCCLK = MODOSC; sampling timer
	ADCCTL2 |= ADCRES;                              // 10-bit conversion results
	ADCMCTL0 |= ADCINCH_1;                     		// A1 ADC input select; Vref=AVCC
	ADCIE |= ADCIE0;                       			// Enable ADC conv complete interrupt

	TB0CCTL0 = CCIE;                             	// CCR0 interrupt enabled
	TB0CTL |= TBSSEL__ACLK | MC__UP | TBCLR; 		// ACLK, continuous mode, clear TBR
	TB0CCR0 = 32768;                           		// 1 Hz

	while (1) {
		ADCCTL0 |= ADCENC | ADCSC;              	// Sampling and conversion start
		__bis_SR_register(LPM0_bits | GIE);     	// LPM0, ADC_ISR will force exit
		__delay_cycles(5000);
	}

}

void Software_Trim() {
	unsigned int oldDcoTap = 0xffff;
	unsigned int newDcoTap = 0xffff;
	unsigned int newDcoDelta = 0xffff;
	unsigned int bestDcoDelta = 0xffff;
	unsigned int csCtl0Copy = 0;
	unsigned int csCtl1Copy = 0;
	unsigned int csCtl0Read = 0;
	unsigned int csCtl1Read = 0;
	unsigned int dcoFreqTrim = 3;
	unsigned char endLoop = 0;

	do {
		CSCTL0 = 0x100;                         	// DCO Tap = 256
		do {
			CSCTL7 &= ~DCOFFG;                  	// Clear DCO fault flag
		} while (CSCTL7 & DCOFFG);               	// Test DCO fault flag

		__delay_cycles((unsigned int) 3000 * MCLK_FREQ_MHZ); // Wait FLL lock status (FLLUNLOCK) to be stable
															 // Suggest to wait 24 cycles of divided FLL reference clock
		while ((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0))
			;

		csCtl0Read = CSCTL0;                   		// Read CSCTL0
		csCtl1Read = CSCTL1;                   		// Read CSCTL1

		oldDcoTap = newDcoTap;               		// Record DCOTAP value of last time
		newDcoTap = csCtl0Read & 0x01ff;       		// Get DCOTAP value of this time
		dcoFreqTrim = (csCtl1Read & 0x0070) >> 4;   // Get DCOFTRIM value

		if (newDcoTap < 256)                    	// DCOTAP < 256
				{
			newDcoDelta = 256 - newDcoTap; 			// Delta value between DCPTAP and 256
			if ((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
				endLoop = 1;                   		// Stop while loop
			else {
				dcoFreqTrim--;
				CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim << 4);
			}
		} else                                   	// DCOTAP >= 256
		{
			newDcoDelta = newDcoTap - 256; 			// Delta value between DCPTAP and 256
			if (oldDcoTap < 256)                	// DCOTAP cross 256
				endLoop = 1;                   		// Stop while loop
			else {
				dcoFreqTrim++;
				CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim << 4);
			}
		}

		if (newDcoDelta < bestDcoDelta)         	// Record DCOTAP closest to 256
				{
			csCtl0Copy = csCtl0Read;
			csCtl1Copy = csCtl1Read;
			bestDcoDelta = newDcoDelta;
		}

	} while (endLoop == 0);                      	// Poll until endLoop == 1

	CSCTL0 = csCtl0Copy;                      		// Reload locked DCOTAP
	CSCTL1 = csCtl1Copy;                       		// Reload locked DCOFTRIM
	while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))
		; // Poll until FLL is locked
}

// ADC interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC_VECTOR))) ADC_ISR (void)
#else
#error Compiler not supported!
#endif
{
	switch (__even_in_range(ADCIV, ADCIV_ADCIFG)) {
	case ADCIV_NONE:
		break;
	case ADCIV_ADCOVIFG:
		break;
	case ADCIV_ADCTOVIFG:
		break;
	case ADCIV_ADCHIIFG:
		break;
	case ADCIV_ADCLOIFG:
		break;
	case ADCIV_ADCINIFG:
		break;
	case ADCIV_ADCIFG:
		ADC_Result = ADCMEM0;
		__bic_SR_register_on_exit(LPM0_bits);      	// Clear CPUOFF bit from LPM0
		break;
	default:
		break;
	}
}

// Timer A0 Interrupt Service Routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B0_VECTOR
__interrupt void Timer_A(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
	// Transmit byte, get all data
	while(ADC_Result != 0)
	{
		while (!(UCA0IFG & UCTXIFG));
		UCA0TXBUF = ADC_Result % 10;
		ADC_Result /= 10;
	}
	while (!(UCA0IFG & UCTXIFG));
	UCA0TXBUF = '\n';
	__no_operation();
}

void Init_GPIO() {
	// Configure GPIO
	P1DIR |= BIT0;                           		// Set P1.0/LED to output direction
	P1OUT &= ~BIT0;                                 // P1.0 LED off

	// Configure ADC A1 pin
	P1SEL0 |= BIT1;
	P1SEL1 |= BIT1;
}
