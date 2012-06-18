#
# MidiCtrl - AVR specific makefile
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

CURDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(CURDIR)MidiCtrl.mk

CC             = avr-gcc
CXX            = avr-g++
OBJCOPY        = avr-objcopy
AVR_ARCH       ?= atmega64
LDAVR_ARCH     ?= avrmega64
FLASH_PROTOCOL = jtag2

CFLAGS   += -Os -ffunction-sections -DAVR -I. -mmcu=$(AVR_ARCH) -fshort-enums -fpack-struct -Wall -Werror
CFLAGS   += -Wall -DLITTLE_ENDIAN -g -flto

CFLAGS += no-tree-loop-optimize -ffreestanding -morder1 -funsigned-char -funsigned-bitfields -fshort-enums -fpack-struct
CFLAGS += -fdata-sections -fno-split-wide-types -fno-inline-small-functions

CLDFLAGS += -Wl,--relax,--gc-sections -ffunction-sections
CLDFLAGS += -mmcu=$(AVR_ARCH)
LDFLAGS  = -m $(LDAVR_ARCH) -M

CXXFLAGS += -std=c++0x

# generate list
# CFLAGS += -Wa,-adhlns=$@.lst

%.avr.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.avr.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

%.avr.o: %.s 
	$(CC) $(CFLAGS) -c $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.avr.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.syx: %.hex
	ihex2sysex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.ee_srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@
