## Shani Thapa, Mitchell Hay 
* Added Code 
* Populated README on 11/21

## Software
### ADC
The ADC was the main software component of this lab. Configuring the ADC required alot of new registers and variables to be manipulated just like UART and Timers. The process should be similar but with new variables and registers.   

## Hardware
The hardware component of the lab was to use to sensors to convert physical phenomena into electrical ones. For this purpose, a different circuit was built around each of the three sensors: photoresistor, temperature sensor, and phototransistor. 

### Photoresistor
A photoresistor is a resistor that changes its resistance depending on the amount of light that interacts with the sensor. The circuit for this sensor was the simple voltage divider. A voltage will be obtained and read by the ADC. 
![Alt text](https://user-images.githubusercontent.com/31711430/33094740-27ec258a-cecf-11e7-869e-a9bdcac11cf9.PNG)

### Temperature Sensor
The LM35 is a temperature sensor that doesn't even require a true circuit to use. Simple connect the IC to ground, power and then read its output. The output voltage can then be converted to degrees celsiu. The conversion is 1C/10mV.  

### Phototransistor  
A phototransistor is a light-sensitive transitor. The sensor can generate current when light is absorbed. The sensor can operate in two modes: photovoltaic and photoconductive. When the transistor is zero-biased, then its in photovoltaic mode where the current is restricted. Meanwhile, the sensor is in photoconductive mode when it's reverse-biased. A common-emitter circuit was built with the phototransistor to get an output voltage. 