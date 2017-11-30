## Shani Thapa, Mitchell Hay
* Code added on 11/8
* README populated on 11/21

## RGB LED
One way to visually see how sensors are changing signals was to use the MSP430 to power an RGB LED based on the ambient temperature around a thermistor. As the temperature around the thermistor increased, the RGB LED would show a warmer color, and vice versa for a decreasing temperature. The color spectrum used for temperature can be seen in the figure below. The temperature range was 0 C to 100 C. To show increasing and decreasing temperatures, outside forces were implemented (hair dryer for hot, ice pack for cold).
![RGB Spectrum](https://github.com/RU09342/lab-5-sensing-the-world-around-you-mitchell-hay/blob/master/Visualizing%20Data/Images/rgb%20spectrum.png)

## LCD Display 
The FR6989 was the only board with an LCD display, so this board must be used for this part of the lab. Once again, the LCD must be initialized like Timers, UART, etc. We are given the LCD driver folder which provides code that facilitates the displaying of characters and numbers to the LCD. We decided to output the temperature into the LCD from the LM35 temperature sensor. The main thing that had to be done in code was to initialize the LCD itself. There are a bunch of variables and registers that must be manipulated for intialization. 

```
{
   LCDCCTL0 &= ~LCDON;                                 // lcd off
   LCDCCTL0 = LCDDIV_2 + LCDPRE__4 + LCD4MUX + LCDLP;  // Flcd = 512 Hz; Fframe = 64 Hz
   LCDCMEMCTL = LCDCLRM;                               // clear memory
   //LCDCVCTL = LCD2B;                                 // 1/3 bias
   LCDCPCTL0 = 0xFFFF;                                 // enable Segments
   LCDCPCTL1 = 0xFFFF;                                 // enable segments
   LCDCPCTL2 = 0x7FFF;                                 // enable segments
   LCDCCTL0 |= LCDON;                                  // lcd on
}
```    
This was the main code we used for initialize the code. Most of it was taken from TI using the Resource manager. Afterwards we simply recylced the code we used for Lab 5 getting data from the LM35. Then we used the function provided for this part to display the characters. The correct temperature was displayed, although there seemed to be a ghosting issue with the LCD. Unfortunetly, we were not able to fix the problem after multiple attempts.  

![Alt text](https://user-images.githubusercontent.com/31711430/33097203-5cdd7ffc-ced7-11e7-9534-d9d8c70c0208.jpg) 

## PC Visualization
There are several different ways to read back data from the serial output of the MSP430. The programs we have used in the past were Realterm and PuTTy, however there are other programs that could read the data, then do something with it. One of those programs is Python and the PySerial library, which were used in this lab. Using this library, data values were able to be read in, stored in a Python program, and manipulated. 
