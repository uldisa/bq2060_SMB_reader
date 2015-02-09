MY_ARD_LOCATIONS= \
/cygdrive/c/PROGRA~1/Arduino \
/home/uldisa/Arduino/build/linux/work

# Take first found Arduino location. No need to change Makefile when changin development site
ARD_HOME=$(word 1, $(foreach ard,$(MY_ARD_LOCATIONS),$(wildcard $(ard))))

MY_USER_LIB_LOCATIONS=/cygdrive/c/DOCUME~1/test/MYDOCU~1/Arduino/LIBRAR~1
USER_LIB_PATH=$(word 1, $(foreach ard,$(MY_USER_LIB_LOCATIONS),$(wildcard $(ard))))

ARDMK_PATH=$(ARD_HOME)/hardware/tools/avr

CPPFLAGS=-mmcu=$(CPU) -DF_CPU=16000000L -DARDUINO=154 #-DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR
LDFLAGS=-mmcu=$(CPU) -Wl,--gc-sections 

CC_NAME      = avr-gcc
CXX_NAME     = avr-g++
OBJCOPY_NAME = avr-objcopy
OBJDUMP_NAME = avr-objdump
AR_NAME      = avr-ar
SIZE_NAME    = avr-size
NM_NAME      = avr-nm

# Names of executables
CC      = $(ARDMK_PATH)/bin/avr-gcc
CXX     = $(ARDMK_PATH)/bin/avr-g++
AS      = $(ARDMK_PATH)/bin/$(AS_NAME)
OBJCOPY = $(ARDMK_PATH)/bin/avr-objcopy
OBJDUMP = $(ARDMK_PATH)/bin/avr-objdump
AR      = $(ARDMK_PATH)/bin/avr-ar
SIZE    = $(ARDMK_PATH)/bin/avr-size
NM      = $(ARDMK_PATH)/bin/avr-nm

CC_CMD=$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
CXX_CMD=$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
ifneq ($(filter %-pc-cygwin,$(MAKE_HOST)),)
CC_CMD=$(CC) $(CPPFLAGS) $(CFLAGS) -c `cygpath -m $<` -o $(foreach f,$@,`cygpath -m $(f)`)
CXX_CMD=$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c `cygpath -m $<` -o $(foreach f,$@,`cygpath -m $(f)`)
endif
%.cpp: %.ino
	echo '#line 1 "$<"' > $@
	echo '#include "Arduino.h"' >> $@
	echo 'void setup();' >> $@
	echo 'void loop();' >> $@
	echo '#line 1 ' >> $@
	cat $< >> $@	

%.elf: %.o 
	$(CC) $(LDFLAGS) -o $@ $^ -lc -lm

%.eep: %.elf
	$(OBJCOPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eprom=0 $< $@ 

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@ 
	$(SIZE) -C $< --mcu=$(CPU)

%.prog: %.hex
ifeq ($(filter %-pc-cygwin,$(MAKE_HOST)),)
	$(ARDMK_PATH)/../avrdude -v -v -p $(CPU) -c$(PROGRAMMER) -P$(PORT) -b$(BR) -V -U flash:w:$<:i -C $(ARDMK_PATH)/../avrdude.conf
else
	@CFG="`cygpath -m $(ARDMK_PATH)/etc/avrdude.conf`" ; \
	PS4=; \
	set -x; \
	$(ARDMK_PATH)/bin/avrdude -v -v -v -v -p $(CPU) -c$(PROGRAMMER) -P$(PORT) -b$(BR) -V -U flash:w:$<:i -C$$CFG 
endif

########################

print-%:
	@echo "$($(*))"
clean::
	rm -f *.o *.elf *.epp *.hex

OBJDIR=.

monitor:
ifeq ($(filter %-pc-cygwin,$(MAKE_HOST)),)
	cu -l $(PORT) -s 115200	
else
	cu -l $(PORT) -s 115200
endif
