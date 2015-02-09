include Arduino.mk

LIBS=core I2C
PORT=COM13
CPU=atmega328p
BR=57600
CPPFLAGS+=-DARDUINO_AVR_PRO -DARDUINO_ARCH_AVR
VARIANT=standard
PROGRAMMER=arduino

USER_LIB_PATH=$(CURDIR)
include lib.mk

CPPFLAGS+=-Wall -Wextra -I. -Os -fno-exceptions -ffunction-sections -fdata-sections

SMB.elf: SMB.o twimaster.o libcore.a 

i2cmaster.o: ASFLAGS+=-x assembler-with-cpp 

test_i2cmaster.elf:test_i2cmaster.o twimaster.o libcore.a

