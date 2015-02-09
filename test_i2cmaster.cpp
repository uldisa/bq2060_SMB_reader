/****************************************************************************
Title:    Access serial EEPROM 24C02 using I2C interace
Author:   Peter Fleury <pfleury@gmx.ch> http://jump.to/fleury
File:     $Id: test_i2cmaster.c,v 1.2 2003/12/06 17:07:18 peter Exp $
Software: AVR-GCC 3.3
Hardware: AT90S8515 at 4 Mhz, any AVR device can be used

Description:
    This example shows how the I2C library i2cmaster.S can be used to 
    access a serial eeprom.
    Based on Atmel Application Note AVR300, adapted to AVR-GCC C interface

*****************************************************************************/
#include <Arduino.h>
#include <avr/io.h>
extern "C" {
#include "i2cmaster.h"
}

#define Dev24C02  0xA0      // device address of EEPROM 24C02, see datasheet
//#define Dev24C02  0xA2      // device address of EEPROM 24C02, see datasheet

char hp_eeprom [] ={
0x7F,0x3C,0xA,0x0,0xB8,0x1,0x0,0xA,0x20,0x91,0xA0,0x41,0x80,0x0,0x00,0x0
,0x0,0x0,0xD0,0x39,0x21,0x0,0xEC,0x2E,0x0,0x2B,0xB8,0xB,0x64,0x0,0xF4,0x1
,0x8,0x47,0x59,0x4E,0x41,0x50,0x41,0x43,0x4B,0xFF,0xFF,0x17,0x0,0x0,0x0,0xFE
,0x4,0x30,0x32,0x4B,0x54,0xFF,0xFF,0xFF,0x30,0x11,0x30,0x11,0x28,0xF1,0x0,0xB7
,0x4,0x4C,0x49,0x4F,0x4E,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xA8
,0x7,0x1,0x2D,0x50,0xD,0x64,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x91,0x26
,0xA0,0xDF,0x0,0xE6,0x7,0xFD,0xB0,0x4F,0x11,0x7C,0x7A,0x53,0x68,0xC5,0x78,0xC4
,0x48,0xC2,0xE0,0x2E,0x13,0x3B,0x2D,0xB,0x5E,0x9,0xBC,0x2,0x68,0x2,0x5A,0xA5
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0x0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

int main(void)
{
    unsigned char ret;
    

    init();
    DDRB  = 0xff;                              // use all pins on porut B for output 
    PORTB = 0xff;                              // (active low LED's )
    Serial.begin(9600);
    i2c_init();                                // init I2C interface
    Serial.print("Start");
       //i2c_stop();                            // set stop conditon = release bus

    /* write 0x75 to eeprom address 0x05 (Byte Write) */
/*    i2c_start_wait(Dev24C02+I2C_WRITE);       // set device address and write mode
    Serial.println("-");
    ret = i2c_start(Dev24C02+I2C_WRITE);       // set device address and write mode
    Serial.println(".");
    if ( ret ) {
        i2c_stop();
    	Serial.println("fail");
	Serial.flush();
        PORTB=0x00;                            // activate all 8 LED to show error
    }else {
*/        /* issuing start condition ok, device accessible */
//    i2c_start_wait(Dev24C02+I2C_WRITE);       // set device address and write mode
 //       i2c_write(0x00);                       // write address = 5
 //      i2c_stop();                            // set stop conditon = release bus
    	Serial.println("init ok");

        /* write ok, read value back from eeprom address 0x05, wait until 
           the device is no longer busy from the previous write operation */
/*        i2c_start_wait(Dev24C02+I2C_WRITE);     // set device address and write mode
        i2c_write(0x05);                        // write address = 5
        i2c_rep_start(Dev24C02+I2C_READ);       // set device address and read mode
        ret = i2c_readNak();                    // read one byte
        i2c_stop();
        
        PORTB = ~ret;                           // output byte on the LED's
*/
        /* write 0x70,0x71,072,073 to eeprom address 0x00..0x03 (Page Write),
           wait until the device is no longer busy from the previous write operation */
/*        i2c_start_wait(Dev24C02+I2C_WRITE);     // set device address and write mode
        i2c_write(0x00);                        // write start address = 0
        i2c_write(0x70);                        // write data to address 0
        i2c_write(0x71);                        //    "    "   "    "    1
        i2c_write(0x72);                        //    "    "   "    "    2
        i2c_write(0x74);                        //    "    "   "    "    3
        i2c_stop();                             // set stop conditon = release bus
  */  
        /* write ok, read value back from eeprom address 0..3 (Sequencial Read),
           wait until the device is no longer busy from the previous write operation */


	for(int y=0;y<32;y++) {
        i2c_start_wait(Dev24C02+I2C_WRITE);      // set device address and write mode
        i2c_write(y*8);                         // write address = 0
        for(int i=0;i<8;i++) {
		i2c_write(hp_eeprom[y*8+i]);	
		delay(10);
	}
        i2c_stop();                              // set stop condition = release bus
    	Serial.println("stop");
       }

        i2c_start_wait(Dev24C02+I2C_WRITE);      // set device address and write mode
        i2c_write(0x00);                         // write address = 0
        i2c_rep_start(Dev24C02+I2C_READ);        // set device address and read mode
	int c;
        for(int i=0;i<256;i++) {
		if (serialEventRun) serialEventRun();
    //    	Serial.print((char)i2c_readAck());                       // read one byte form address 0
        	Serial.print(",0x");                       // read one byte form address 0
		c=i2c_readAck();	
		//if(c>=32 && c<=127){
        	//Serial.print((char)c);                       // read one byte form address 0
		//} else {
        	//Serial.print(" ");                       // read one byte form address 0
        	Serial.print(c,HEX);                       // read one byte form address 0
		//}
		if((i&0xf)==0xf) {
			Serial.println("");
		}
	}
        i2c_stop();                              // set stop condition = release bus
    	Serial.println("stop");
    
        //PORTB = ~ret;                            // output byte on the LED's        
    //}
    
    for(;;);	
}
