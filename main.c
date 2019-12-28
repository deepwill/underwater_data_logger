//**********************************************************
//**********************************************************
//***********Underwater Logger******************************
//**********************************************************
//Fat routine taken from here: https://www.mikrocontroller.net/articles/AVR_FAT32


#define F_CPU 8000000UL       //freq 8 MHz

//change this to true to go to sleep mode
#define USE_SLEEP_MODE FALSE

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "DATA_routines.h"
//#include "I2C_routines.h"
#include "ADC_routines.h"


//new
#include "mmc_config.h" // configuration among other for correct file system interaction
#include "file.h"
#include "fat.h"
#include "mmc.h"


static void timer1_init(void);

#if(F_CPU == 8000000)           // error 0,16%
#define TOP_OCR 0x4D
#define START_TCNT 0xB2
#define PRESCALER 0x05
#endif


volatile uint8_t    TimingDelay;    // for mmc.c
ISR (TIMER1_COMPA_vect)
{
    TimingDelay = (TimingDelay==0) ? 0 : TimingDelay-1;
}

/****************************************/
//timer inits
/****************************************/
static void timer1_init(){

    TimingDelay = 0;        // initalizaton of counter variable   

    TCCR1B = 1<<WGM12;      // timer0 in ctc mode
    TIMSK |= 1<<OCIE1A;      // compare interrupt on

    TCNT0 = START_TCNT;     // init start counter
    OCR1A = TOP_OCR;        // maximum before compare match activation 

    TCCR1B = PRESCALER;     // when prescaler is set timer always starts to run
    sei();                  // interrupts on
}


volatile uint8_t timer2Counter;

//char mybuffer[4]; //for itoa conversion

void markOnSdCard (uint8_t *);
char  *ft_itoa(int);

void port_init(void)
{
    //PORTB = 0xEF;
    DDRB  = 0xEF; //MISO line i/p, rest o/p
    PORTC = 0x00;

    DDRD |= (1<<PD6)|(1<<PD5);//output PD6 and 5 for status led green and red
    PORTD &=~ (1<<PD6)|(1<<PD5);//turn off output led per default

    DDRD  &=~ (1<<PD3)|(1<<PD7);//inputs: PD3 for jumper mode selector pd7 for button
    PORTD &=~ (1<<PD3);//pull down for pd3 when jumper to vcc enter in user interface mode 
}

void timer_init(void)
{
    //TCCR0 |= (1<<CS02)|(1<<CS00); //1024 prescaling
    //TIMSK |=(1<<TOIE0); //enable overflow interrupt
#if defined(__AVR_ATmega8A__)
    //asynchronous timer 2 init
    TIMSK |= (1<<TOIE2); //enable overflow interrupt for timer2
    TCCR2 |= (1<<CS22)|(1<<CS20);//prescaler 128 for timer 2 -> overflow every second
    ASSR |= (1<<AS2);//asynchronous operation timer2
#endif
#if defined(__AVR_ATmega88p__)
    //asynchronous timer 2 init
    TIMSK2 |= (1<<TOIE2); //enable overflow interrupt for timer2
    TCCR2B |= (1<<CS22)|(1<<CS20);//prescaler 128 for timer 2 -> overflow every second
    ASSR |= (1<<AS2);//asynchronous operation timer2
#endif

}

//call this routine to initialize all peripherals
void init_devices(void)
{
    cli();
    port_init();
    //spi_init();
    uart_init();
    ADC_Init();
    timer_init();
    //i2c_init();//no i2c module at moment
    sei();

    //all peripherals are now initialized
}


int main(void) {
    uint16_t * tempArray = malloc(10*sizeof(uint16_t));
    uint8_t * lightArray = malloc(10*sizeof(uint8_t));
    uint8_t batchCounter =0;
    int absCounter = 0;
    uint8_t minCounter =0;
    _delay_ms(100);  //delay for VCC stabilization

    uint8_t file_name [] = "log.txt";



    // this string is used to write to the sd card
#if (MMC_WRITE == TRUE)

    char str[15];
    //char str1[15];
    //char *str1 = malloc(15*sizeof(char));
    char str2[15];
#endif


    init_devices();
    timer1_init();

    TX_NEWLINE;
    TX_NEWLINE;
    uart_puts("***********************************");
    TX_NEWLINE;
    uart_puts("UW Logger");
    TX_NEWLINE;
    uart_puts("***********************************");
    TX_NEWLINE;



    //****Evaluate Jumper Position***//
    /* uint8_t selectedMode=0;
       if (PIND &(1<<PD3)) {
       cli();
       red_led_on
    //InterfacingwUser();
    }*/
    //*******************************//

    // sd/mmc config  **************************************************
    if( FALSE == mmc_init() ){
        return;
    } 

    uart_puts("mmc_config done\n\r");

    // fat config ******************************************************
    if( FALSE == fat_loadFatData() ){
        return;
    }

    uart_puts("fat config done \n\r");


    markOnSdCard(file_name);    
    while (1) {

        /*        green_led_on
                  _delay_ms(500);
                  green_led_off*/

#if (USE_SLEEP_MODE)
        set_sleep_mode(SLEEP_MODE_PWR_SAVE); //power slave for asy_clk wakeup
        cli();
        sleep_enable();
        sei();
        sleep_cpu();
        _delay_ms(50);
        sleep_disable();
#endif
        //***********


        //	***making minute mark
        if (timer2Counter >= 60){
            timer2Counter=0;
            minCounter++;
        }
        //***every 5 minutes
        if (minCounter >= 5) {
            minCounter=0;

            //tempArray[batchCounter]=adc_singleMeasure();
            //lightArray[batchCounter]=adc_lightIntens();

            if(batchCounter<=9){//save to flash
                tempArray[batchCounter]=adc_singleMeasure();
                lightArray[batchCounter]=adc_lightIntens();
                red_led_on
                    _delay_ms(500);
                red_led_off
                    batchCounter++;    
            }else{//write to sd card
                batchCounter=0;
                red_led_off 




#if (MMC_WRITE ==TRUE)      // create and append only if write is TRUE
                // ****************************************************
                // if file exists, it will be opened and then append to it.
                if( MMC_FILE_OPENED == ffopen(file_name,'r') ){
                    // uart_puts("in write segment");
                    ffseek(file.length);
                    for(uint8_t ii=0; ii<7; ii++){
                        itoa(tempArray[ii],str,10);
                        ffwrites(str);//ft_itoa(adc_singleMeasure()));//&tempArray[0]));

                        ffwrite(0x3B);//simicolon
                        itoa(lightArray[ii],str2,10);
                        ffwrites(str2);//ft_itoa(adc_lightIntens()));//&lightArray[0]));
                        ffwrite(0x0D);// new line in file
                        ffwrite(0x0A);
                    }
                    ffclose();
                    uart_puts("existing file written\n\r");
                    green_led_on
                        _delay_ms(500);
                    green_led_off

                }

                // ***************************************************
                // if the file does not exist, it will be created and written to it.
                if(MMC_FILE_CREATED == ffopen(file_name,'c') ){

                    ffwrite(0x0D);      // new line in file
                    ffwrite(0x0A);
                    ffclose();
                    uart_puts("file new created\n\r");
                }
#endif
            }//***batch condition end

        }//*******timer loop end
    } //**********timer2 counter condition end

}
char  *ft_itoa(int n)
{
    int     digits;
    int     number;
    char    ip;
    char    *str;

    digits = 0;
    number = n;
    while(number > 0)
    {
        number = number/10;
        digits++;
    }
    str = (char*)malloc(sizeof(char)*(digits + 1));
    str[digits + 0] = '\0';
    number = n;
    while (number)
    {
        ip = number % 10 + '0';
        str[digits - 1] = ip;
        digits--;
        number = number/10;
    }
    return (str);
}

//********** END *********************

ISR(TIMER2_OVF_vect){
    //ISR routine for timer2 executed every second
    //this is executed every second. 32768hz comes form external crystal source
    timer2Counter++; //increment timer2Counter
}


/****************************************/
//use this  function to insert a mark which allows to post syncronize the time information by the user
/****************************************/
void markOnSdCard (uint8_t * fn){

    if( MMC_FILE_OPENED == ffopen(fn,'r') ){
        ffseek(file.length);
        ffwrites("restarted");
        ffwrite(0x0D);// new line in file
        ffwrite(0x0A);
        ffclose();
    }
    // if the file does not exist, it will be created and written to it.
    if(MMC_FILE_CREATED == ffopen(fn,'c') ){
        ffwrites("restarted");
        ffwrite(0x0D);      // new line in file
        ffwrite(0x0A);
        ffclose();
        uart_puts("file new created\n\r");
    }

    green_led_on
        _delay_ms(500);
    green_led_off
}

