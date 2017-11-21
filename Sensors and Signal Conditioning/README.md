## Shani Thapa, Mitchell Hay 
* Added Code 
* Populated README on 11/21

## Software
### ADC
The ADC was the main software component of this lab. Configuring the ADC required alot of new registers and variables to be manipulated just like UART and Timers. The process should be similar but with new variables and registers. 
*ADCxxCTLx register selects the ADC input channel and the ADC clock division
*ADCxxIE enables interrupts for the specific ADC
*ADCxxON turn on the specific ADC 
*ADCxxSHTxx is the sample and hole time. These bits define the number of clock cycles in ADC 
*ADCxxSHP is the sample and hold pulse-mode select, decides if the source of the sampling signal is input or output 
*ADCxxENC enables conversions 
*ADCxxSC starts conversions 

#### Code Example for 12-Bit ADC 
```
\\FR5529 
{
	ADC12CTL0 = ADC12SHT02 + ADC12ON;         // Sampling time, ADC12 on
	ADC12CTL1 = ADC12SHP;                     // Use sampling timer
	ADC12IE = 0x01;                           // Enable interrupt
	ADC12CTL0 |= ADC12ENC;                    // enables conversions
}
## Hardware
The hardware component of the lab was to use to sensors to convert physical phenomena into electrical ones. For this purpose, a different circuit was built around each of the three sensors: photoresistor, temperature sensor, and phototransistor. 

### Photoresistor
A photoresistor is a resistor that changes its resistance depending on the amount of light that interacts with the sensor. The circuit for this sensor was the simple voltage divider. A voltage will be obtained and read by the ADC. 
![Alt text](https://user-images.githubusercontent.com/31711430/33094740-27ec258a-cecf-11e7-869e-a9bdcac11cf9.PNG)

Vout = Vin*(R2)/(R1+R2)
This is the voltage divider equation which calculates the voltage that can be obtained from the circuit. As the amount of light on the photoresistor changes, the output voltage will also change.  

### Temperature Sensor
The LM35 is a temperature sensor that doesn't even require a true circuit to use. Simple connect the IC to ground, power and then read its output. The output voltage can then be converted to degrees celsiu. The conversion is 1C per 10mV output.  

![Alt test](https://user-images.githubusercontent.com/31711430/33095648-587cdb42-ced2-11e7-979a-1c42856c36de.PNG)

### Phototransistor  
A phototransistor is a light-sensitive transitor. The sensor can generate current when light is absorbed. The sensor can operate in two modes: photovoltaic and photoconductive. When the transistor is zero-biased, then its in photovoltaic mode where the current is restricted. Meanwhile, the sensor is in photoconductive mode when it's reverse-biased. A common-emitter circuit was built with the phototransistor to get an output voltage. 

![Alt text](https://user-images.githubusercontent.com/31711430/33094753-2de4b736-cecf-11e7-96f2-d7351178b7ae.PNG)

In this configuration, the voltage remains constant when the transistor's exposure to light remains constant. As more light is absorbed by the phototransistor though, the output voltage will decrease. 