/*
 * Mitchell Hay and Shani Thapa
 * RU09342 - 4
 * Lab 5 - RGB LED
 * MSP430F5529
 */
//******************************************************************************
//   MSP430F552x Demo - ADC12, Sample A0, Set P1.0 if A0 > 0.5*AVcc
//
//   Description: A single sample is made on A0 with reference to AVcc.
//   Software sets ADC12SC to start sample and conversion - ADC12SC
//   automatically cleared at EOC. ADC12 internal oscillator times sample (16x)
//   and conversion. In Mainloop MSP430 waits in LPM0 to save power until ADC12
//   conversion complete, ADC12_ISR will force exit from LPM0 in Mainloop on
//   reti. If A0 > 0.5*AVcc, P1.0 set, else reset.
//
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

// Maxmium Red at 100 Degrees Celcius
// Maximum Purple at 0 Degrees Celcius

#include <msp430f5529.h>

volatile int val = 0;
volatile unsigned int chan0;
volatile unsigned int chan1;
volatile unsigned int r = 0;
volatile unsigned int g = 0;
volatile unsigned int b = 0;
volatile unsigned int result;
float percentage;
int diff1 = 0;
int diff2 = 0;

int main(void) {

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	TA0CCR0 = 256 - 1;                        	// PWM Period
	TA0CTL = TASSEL_2 + MC_1 + TACLR;         	// SMCLK, up mode, clear TAR

	P6SEL = 0x0F;                             // Enable A/D channel inputs
	ADC12CTL0 = ADC12ON + ADC12MSC + ADC12SHT0_8; // Turn on ADC12, extend sampling time
												  // to avoid overflow of results
	ADC12CTL1 = ADC12SHP + ADC12CONSEQ_3; // Use sampling timer, repeated sequence
	ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
	ADC12MCTL1 = ADC12INCH_1;                 // ref+=AVcc, channel = A1
	ADC12MCTL2 = ADC12INCH_2;                 // ref+=AVcc, channel = A2
	ADC12MCTL3 = ADC12INCH_3 + ADC12EOS;    // ref+=AVcc, channel = A3, end seq.
	ADC12IE = 0x08;                           // Enable ADC12IFG.3
	ADC12CTL0 |= ADC12ENC;                    // Enable conversions
	ADC12CTL0 |= ADC12SC;                     // Start convn - software trigger

	// PWM Setup
	P1DIR |= BIT2 + BIT3 + BIT4;		// Set up Pins 1.2,3, and 4 as outputs
	P1SEL |= BIT2 + BIT3 + BIT4;	// Set Pins 1.2,3, and 4 to Timer A0 CCRx
	TA0CCTL1 = OUTMOD_7;                      	// CCR1 reset/set
	TA0CCR1 = r;                            	// CCR1 PWM duty cycle, red LED
	TA0CCTL2 = OUTMOD_7;                      	// CCR2 reset/set,
	TA0CCR2 = g;                            // CCR2 PWM duty cycle, green LED
	TA0CCTL3 = OUTMOD_7;                      	// CCR3 reset/set
	TA0CCR3 = b;                            	// CCR3 PWM duty cycle, blue LED

	P6SEL |= 0x03;                            // P6.0 ADC option select
	P1DIR |= 0x01;                            // P1.0 output

	while (1) {
		ADC12CTL0 |= ADC12SC;                   // Start sampling/conversion

		__bis_SR_register(GIE);     // LPM0, ADC12_ISR will force exit
		//__no_operation();                       // For debugger
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
	  switch(__even_in_range(ADC12IV,34))
	  {
	  case  0: break;                           // Vector  0:  No interrupt
	  case  2: break;                           // Vector  2:  ADC overflow
	  case  4: break;                           // Vector  4:  ADC timing overflow
	  case  6: break;                           // Vector  6:  ADC12IFG0
	  case  8: break;                           // Vector  8:  ADC12IFG1
	  case 10: break;                           // Vector 10:  ADC12IFG2
	  case 12:                                  // Vector 12:  ADC12IFG3
		  diff1 = val-ADC12MEM1;
		  diff2 = ADC12MEM1-val;
	          // Set up buffer so LED isn't constantly changing
		  if ((diff1 >= 10) || (diff2 >= 10)){
		  val = ADC12MEM1;
		  // Blue Region -> Purple Region
		  	if (val <= 3100 && val >= 2530)
		  	{
		  		percentage = ((float)(val - 2530)/571);
		  		r = 0xFF * percentage;
		  		g = 0x00;
		  		b = 0xFF;
		  	}

		  	// Teal Region -> Blue Region
		  	else if (val <= 2529 && val >= 1958)
		  	{
		  		percentage = ((float)(2529 - val)/571);
		  		r = 0x00;
		  		g = 0xFF * percentage;
		  		b = 0xFF;
		  	}

		  	// Green Region -> Teal Region
		  	else if (val <= 1957 && val >= 1386)
		  	{
		  		percentage = ((float)(val - 1386)/571);
		  		r = 0x00;
		  		g = 0xFF;
		  		b = 0xFF * percentage;
		  	}

		  	// Yellow Region -> Green Region
		  	else if (val <= 1385 && val >= 814)
		  	{
		  		percentage = ((float)(1385 - val)/571);
		  		r = 0xFF * percentage;
		  		g = 0xFF;
		  		b = 0x00;
		  	}

		  	// Yellow Region -> Red Region
		  	else if (val <= 813 && val >= 242)
		  	{
		  		percentage = ((float)(val - 242)/571);
		  		r = 0xFF;
		  		g = 0xFF * percentage;
		  		b = 0x00;
		  	}
			  // If greater than 100C
		  	else if (val < 242)
		  	{
		  		r = 0xFF;
		  		g = 0x00;
		  		b = 0x00;
		  	}
			  // If lower than 0C
		  	else if (val > 3100)
		  	{
		  		r = 0xFF;
		  		g = 0x00;
		  		b = 0xFF;
		  	}
			  // Set RGB LED
		  TA0CCR1 = r;
		  TA0CCR2 = g;
		  TA0CCR3 = b;
		  }

	  case 14: break;                           // Vector 14:  ADC12IFG4
	  case 16: break;                           // Vector 16:  ADC12IFG5
	  case 18: break;                           // Vector 18:  ADC12IFG6
	  case 20: break;                           // Vector 20:  ADC12IFG7
	  case 22: break;                           // Vector 22:  ADC12IFG8
	  case 24: break;                           // Vector 24:  ADC12IFG9
	  case 26: break;                           // Vector 26:  ADC12IFG10
	  case 28: break;                           // Vector 28:  ADC12IFG11
	  case 30: break;                           // Vector 30:  ADC12IFG12
	  case 32: break;                           // Vector 32:  ADC12IFG13
	  case 34: break;                           // Vector 34:  ADC12IFG14
	  default: break;
	  }
}
