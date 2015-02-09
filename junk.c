#define F_CPU 16000000UL	
#include <util/delay.h>
#include <avr/io.h>
#include "uart.c"

static FILE mystdout = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

#define LED PORTB5
// ---------------------------------------------------------------------------
// I2C (TWI) ROUTINES
//
// On the AVRmega series, PA4 is the data line (SDA) and PA5 is the clock (SCL
// The standard clock rate is 100 KHz, and set by I2C_Init. It depends on the AVR osc. freq.

#define F_SCL 100000L // I2C clock speed 100 KHz
#define ReadBit 1  //low bit of device address for read
#define TW_START 0xA4 // send start condition (TWINT,TWSTA,TWEN)
#define TW_STOP 0x94 // send stop condition (TWINT,TWSTO,TWEN)
#define TW_ACK 0xC4 // return ACK to slave
#define TW_NACK 0x84 // don't return ACK to slave
#define TW_SEND 0x84 // send data (TWINT,TWEN)
#define TW_READY (TWCR & 0x80) // ready when TWINT returns to logic 1.
#define TW_STATUS (TWSR & 0xF8) // returns value of status register
#define I2C_Stop() TWCR = TW_STOP // inline macro for stop condition

void I2C_Init(){

// at 16 MHz, the SCL frequency will be 16/(16+2(TWBR)), assuming prescalar of 0.
// so for 100KHz SCL, TWBR = ((F_CPU/F_SCL)-16)/2 = ((16/0.1)-16)/2 = 144/2 = 72.
    TWSR = 0; // set prescalar to zero
    TWBR = ((F_CPU/F_SCL)-16)/2; // set SCL frequency in TWI bit register
  PORTC = (1 << PORTC4) | (1 << PORTC5); //enable pullups
}

unsigned char I2C_Detect(unsigned char addr){
// look for device at specified address; return 1=found, 0=not found
//	printf("\r\naddr %02x",addr);
    TWCR = TW_START; // send start condition
    while (!TW_READY); //wait
	TWDR = addr; // load device's bus address
    TWCR = TW_SEND; // and send it
    while (!TW_READY);
//	 printf(" %02x ",TW_STATUS);
    return (TW_STATUS==0x18); // return 1 if found; 0 otherwise
}

unsigned char I2C_FindDevice(unsigned char start){
// returns with address of first device found; 0=not found
    unsigned char addr;
    for (addr=start;addr<0xFE;addr+=2) { // search all 127 addresses
        if (I2C_Detect(addr)) // I2C detected?
        return addr; // leave as soon as one is found
    	}
    return 0; // none detected, so return 0.
}

void ShowDevices(){
// Scan I2C addresses and display addresses of all devices found
    unsigned char addr = 2;
    while (addr>0) {
        addr = I2C_FindDevice(addr);
        if (addr>0) {printf(" .%02X ",addr); addr +=2;}
    }
}


int main( void ) {

	DDRB |= (1 << LED);			// set LED to output
//	PORTC |= (1<<PC4)|(1<<PC5); 			//pullup on SDA, SDC

    uart_init(9600);	
    stdout = &mystdout; //Required for printf init

  printf("Init I2C\r\n");  
  I2C_Init();
  printf("Showing devices");
  ShowDevices();
  printf("\r\ndone\r\n");
	while(1);
}

