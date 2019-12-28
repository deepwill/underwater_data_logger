/*
 * ADC_routines.h
 *
 * Created: 20.05.2016 11:50:17
 *  Author: andre
 */ 


#ifndef ADC_ROUTINES_H_
#define ADC_ROUTINES_H_

#define red_led_on      PORTD |=  (1<<PD6);
#define red_led_off      PORTD &=~ (1<<PD6);
#define red_led_toggle      PORTD ^= (1<<PD6);

#define green_led_on      PORTD |=  (1<<PD5);
#define green_led_off      PORTD &=~ (1<<PD5);
#define green_led_toggle      PORTD ^= (1<<PD5);




//extern void InterfacingwUser (void);

void ADC_Init(void);

uint16_t adc_singleMeasure();
uint16_t adc_lightIntens ();


#endif /* ADC_ROUTINES_H_ */
