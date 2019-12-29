# UW logger
This project is an underwater logger that can be build at a very low cost (<20€ per unit) with widely available materials and resources. In many of our projects with schools we focus on the marine environment. We believe that to monitor certain parameters of surface waters can be very beneficial to learn and understand about this captivating setting

for more information go to: https://deepwill.org/underwater-logger/

## Features
-the logger is based on the Atmega8 microcontroller
-powered with 4 AAA batteries
-the here presented design accommodates for temperature and light-intensity measurements
-in total two i2c sensors can be connected and three analog sensors
-two buttons can be used to interface with the logger
-the data is saved on a microSD card for easy access
-the housing was successfully tested in up to 30m of water depth (but deeper deployments most certainly will work) and will be tested in the future



## Calculation for converting ADC value to Temperature Unit 

Example calculation: 

ADC reads 635. Vref is 2.56 V. R2 is 10k. Vcc is 3.3V 10bit resolution is chosen: 

1. Calculate voltage that the ADC pin "sees":
Vadc = (ADCvalue * Vref) / 1024 = (635 * 2.56V) /1024 = 1.58V

2. Calculate Resistance

Rthermistor = (Vcc * R2 / Vadc) - R2 = (3.3V * 10K / 1.58V) - 10K = 10.88 K

3. Calculate or lookup Value for this specific thermistor: 
Example for Stein-Hart model:
used Thermistor in this example is: B57863S0103+040 tbd tolerance 0.2 or 0.5K

Coefficients:
A=1.127005602 e-3
B=2.344242532 e-4
C=0.8688369221 e-7

T= 1/(A+B*ln(R)+C*(ln(R))^3)
