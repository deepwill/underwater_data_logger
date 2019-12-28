
/****************************************/
/********ADC.c***************************/
/****************************************/
#include <avr/io.h>
#include "stdlib.h"
#include "uart.h"

void ADC_Init(void){
    ADMUX |= (1<<REFS1)|(1<<REFS0); //internal reference of 2.56 with external capacitor at AREF pin
    ADCSRA = (1<<ADPS1) | (1<<ADPS2);     // Frequency divider 64 -> important since ADC freq has to be within 50 and 200 khz 
    ADCSRA |= (1<<ADEN);                  // ADC activate


    ADCSRA |= (1<<ADSC);                  // one ADC-operation
    while (ADCSRA & (1<<ADSC) ) {	 }    // wait until convertion ready
}

uint16_t adc_singleMeasure (void){
    ADMUX = (ADMUX & ~(0x1F)) | (0x2); // PC2 - Thermistor ->maintain ADMUX (set above)  delete four lsb then set second bit equivalent to: (1<<MUX1) 

    ADCSRA |= (1<<ADSC);            // one conversion "single conversion"
    while (ADCSRA & (1<<ADSC)) {}   // wait until conversion ready


    char thermBuffer[10];
    utoa(ADCW,thermBuffer,10);
    uint16_t intThermistor=atoi(thermBuffer);

    uart_puts("Thermistor [dn]: ");
    uart_puts(thermBuffer);
    uart_puts("\n\r");
    return intThermistor;                    // ADC readout and return 

}

uint16_t adc_lightIntens (void){
    ADMUX = (ADMUX & ~(0x1F)) | (0x0); // PC0 - light intensity ->maintain ADMUX (set above)  delete four lsb then set second bit equivalent to: (1<<MUX1) 

    ADCSRA |= (1<<ADSC);            // one conversion "single conversion"
    while (ADCSRA & (1<<ADSC)) {}   // wait until conversion ready


    char litIntensBuffer[10];
    utoa(ADCW,litIntensBuffer,10);
    uint16_t intlitIntents=atoi(litIntensBuffer);

    uart_puts("Light intensity measurement [dn]");
    uart_puts(litIntensBuffer);
    uart_puts("\n\r");

    return intlitIntents;                    // read ADC and return value

}
