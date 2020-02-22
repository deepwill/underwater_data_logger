/*
 * DATA_routines.c
 *
 * Created: 13.05.2016 20:55:06
 *  Author: andre
 */ 
#include "DATA_routines.h"
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include "uart.h"
#include <util/delay.h>
#include "I2C_routines.h"
#include <avr/interrupt.h>


/*int DATA_return(int inputValue)
{
    //this collects data until 512Bytes have been collected. then it is written to sd card
    int buffersize;

    buffersize=sprintf(buffer + strlen(buffer),"%d",inputValue); //strlen to append array otherwise its overwritten
    return buffersize;

}*/

/*int DATA_returnFloat(float inputValue)
{

    //this collects data until 512Bytes have been collected. then it is written to sd card
    int buffersize;
    int tmpint1=inputValue;
    float tmpFrac = inputValue-tmpint1;
    int tmpint2=tmpFrac*10000;

    buffersize=sprintf(buffer + strlen(buffer),"%d%d",tmpint1,tmpint2); //strlen to append array otherwise its overwritten

    //buffersize=sprintf(buffer + strlen(buffer),"%d",tmpint2); //strlen to append array otherwise its overwritten
    //strcat(buffer,s);
    //itoa(tmpint2,10,s);
    //strcat(buffer,s);


    return buffersize;
}*/


/*void newLine()
{
    //buffer[strlen(buffer)]=
    sprintf(buffer + strlen(buffer),"%s", "\n\r"); //strlen to append array otherwise its overwritten
}*/

/*void returnBlock(unsigned char blockNumber[4])
{
    startBlock = (blockNumber[0] & 0x0f) * 1000;
    startBlock += (blockNumber[1] & 0x0f) * 100;
    startBlock += (blockNumber[2] & 0x0f) * 10;
    startBlock += (blockNumber[3] & 0x0f);
}*/

/*void returnBlockInt(int intNumber)
{
    int mil=intNumber/1000;
    int hundert=(intNumber-mil)/100;
    int dec=(intNumber-mil-hundert)/10;
    int one=(intNumber-mil-hundert-dec);

    startBlock = (mil & 0x0f);
    startBlock += (hundert & 0x0f);
    startBlock += (dec & 0x0f);
    startBlock += (one & 0x0f);	
}*/

/*float oneshotTemp()
{

    //move uarts line to find where code fails
    int tempvalue;
    unsigned char ret;
    char codelsb, codemsb;
    char s[7];
    float decTemp= 0.0;


    //uart_puts("debug2");
    //i2c_init();    // init I2C interface
    //problem here in second call of the start condition::
    ret = i2c_start(ADDRESSTEMP+I2C_WRITE);
    //problem here ::: stays in while loop ??

    if (ret){
        i2c_stop();
        uart_puts("Starting i2c failed");
    }else{
        //i2c_start(0x90);//ADDRESSTEMP+I2C_WRITE);0x48 +0x01 = 0x91
        //**i2c_write(0x0);//to access read only temp register
        //i2c_stop();
        i2c_write(0x03);//config register pointer
        i2c_write(0x20);//one shot

        i2c_stop();
        _delay_ms(250);
        i2c_start_wait(ADDRESSTEMP+I2C_READ);
        //i2c_start(ADDRESSTEMP+I2C_READ);
        codemsb = i2c_readAck();

        codelsb =i2c_readNak();
        i2c_stop();

        //codemsb=0b00000000; //for testing should result in 0x001
        //codelsb=0b00001000;
        tempvalue = ((codemsb<<8)|codelsb)>>3; //13 bit temperature value with 0.0625 degrees resolution. First combine MSB and LSB and then shift 3 to right to ignore unused 3 bits.


        //uart_puts(itoa(tempvalue,s,10)); //output of int result as string in this case for uart

        decTemp = tempvalue/16.0;//Divide by 2 (2.0 is important) because of 0.5 degree LSB resolution or by 16 if one LSB is 0.0625
        dtostrf(decTemp,6,4,s);//convert float to string in order to output to UART
        uart_puts(s);
        uart_puts("\n\r");

        _delay_ms(250);
    }

    return decTemp;//tempvalue;

}*/

/*void InterfacingwUser (void){

    while (1){
        unsigned char option, data;
        unsigned int i;

        TX_NEWLINE;
        uart_puts("Press any key...");
        TX_NEWLINE;
        option = uart_receive();
        TX_NEWLINE;
        uart_puts("> 0 : Erase Blocks");
        TX_NEWLINE;
        uart_puts("> 1 : Write single Block");
        TX_NEWLINE;
        uart_puts("> 2 : Read single Block");
        TX_NEWLINE;
        uart_puts("> 3 : Write 512 bytes to Block");

        TX_NEWLINE;
        //int test = adc_singleMeasure();
        TX_NEWLINE;
        //	_delay_ms(500);


        /*WARNING: If option 0, 1 or 3 is selected, the card may not be detected by PC/Laptop again,
          as it disturbs the FAT format, and you may have to format it again with FAT32.
          This options are given for learning the raw data transfer to & from the SD Card*/

 /*       option = uart_receive();
        uart_putc(option);


        if((option >= 0x30) && (option <=0x32)) //get starting block address for options 0 to 4
        {
            TX_NEWLINE;
            TX_NEWLINE;
            uart_puts("Enter the Block number (0001-9999):");
            data = uart_receive(); uart_putc(data);
            startBlock = (data & 0x0f) * 1000;
            data = uart_receive(); uart_putc(data);
            startBlock += (data & 0x0f) * 100;
            data = uart_receive(); uart_putc(data);
            startBlock += (data & 0x0f) * 10;
            data = uart_receive(); uart_putc(data);
            startBlock += (data & 0x0f);
            TX_NEWLINE;
        }

        totalBlocks = 1;

        switch (option)
        {
            case '0': //error = SD_erase (block, totalBlocks);
                error = SD_erase (startBlock, totalBlocks);
                TX_NEWLINE;
                if(error)
                    uart_puts("Erase failed..");
                else
                    uart_puts("Erased!");
                break;

            case '1': TX_NEWLINE;
                      uart_puts(" Enter text (End with ~):");
                      i=0;
                      do
                      {
                          data = uart_receive();
                          uart_putc(data);
                          buffer[i++] = data;
                          if(data == '\r')    //append 'newline' character whenevr 'carriage return' is received
                          {
                              uart_putc('\n');
                              buffer[i++] = '\n';
                          }
                          if(i == 512) break;
                      }while (data != '~');

                      error = SD_writeSingleBlock (startBlock);
                      TX_NEWLINE;
                      TX_NEWLINE;
                      if(error)
                          uart_puts("Write failed..");
                      else
                          uart_puts("Write successful!");
                      break;

            case '2': error = SD_readSingleBlock (startBlock);
                      TX_NEWLINE;
                      if(error)
                          uart_puts("Read failed..");
                      else
                      {
                          for(i=0;i<512;i++)
                          {
                              if(buffer[i] == '~') break;
                              uart_putc(buffer[i]);
                          }
                          TX_NEWLINE;
                          TX_NEWLINE;
                          uart_puts("Read successful!");
                      }

                      break;

            case '3':
                      i=0;

                      startBlock=blockCounter;

                      //while(bufferSize <512) bufferSize += DATA_return(7777); //test for filling with dummy data
                      //one shot temp measurement:

                      //execute this every 1min
                      while(bufferSize<512){
                          bufferSize += DATA_return(oneshotTemp());
                          bufferSize += DATA_return(adc_singleMeasure());//light measurement
                          newLine();
                          bufferSize +=2; //for length of newline symbol
                          //_delay_ms(240);
                      }

                      if (bufferSize >=511) //once one block has been filled...
                      {
                          blockCounter++;
                          //returnBlockInt(blockCounter);
                          error = SD_writeSingleBlock (startBlock);
                          TX_NEWLINE;
                          TX_NEWLINE;

                          if(error)
                              uart_puts("Write failed..");
                          else
                              uart_puts("Write successful!");
                          break;
                      }else{
                          uart_puts("still not full");
                          break;
                      }

                      while(1)
                      {
                          data = uart_receive();
                          if(data == '\r') break;  //'ENTER' key pressed
                          if(data == 0x08)  //'Back Space' key pressed
                          {
                              if(i != 0)
                              {
                                  uart_putc(data);
                                  uart_putc(' ');
                                  uart_putc(data);
                                  i--;
                              }
                              continue;
                          }
                          if(data <0x20 || data > 0x7e) continue;  //check for valid English text character
                          uart_putc(data);

                          if(i==13){uart_puts(" file name too long.."); break;}
                      }
                      if(i>12) break;

                      TX_NEWLINE;
                      break;


            default: TX_NEWLINE;
                     TX_NEWLINE;
                     uart_puts(" Invalid option!");
                     TX_NEWLINE;
        }

        TX_NEWLINE;
    }
}*/
