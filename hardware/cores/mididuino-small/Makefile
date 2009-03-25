CC=avr-gcc
CXX=avr-g++
OBJCOPY = avr-objcopy
UISP=uisp
AVR_ARCH = atmega32
LDAVR_ARCH = avrmega32
#CFLAGS += -Os -ffunction-sections -DAVR -I. -mmcu=$(AVR_ARCH) -frtl-abstract-sequences -DF_CPU=16000000L
#CFLAGS += -Wall -DLITTLE_ENDIAN -Werror -Winline --param max-inline-insns-auto=2
CFLAGS = -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega32 -DF_CPU=16000000L -I.
CLDFLAGS += -Wl,--gc-sections -mmcu=$(AVR_ARCH)
LDFLAGS = -m $(LDAVR_ARCH) -M
CXXFLAGS = $(CFLAGS)

PROJ=test

all: $(PROJ).hex

# atmega 8 512 word boot size = 0x1c00 
BOOTLOADER8_START = 0x1C00
# atmega 1688 512 word boot size = 
BOOTLOADER168_START = 0x3800
# mega32 2048 word boot size
BOOTLOADER32_START = 0x7000

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp Makefile
	$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.cxx Makefile
	$(CXX) $(CFLAGS) -c $< -o $@

%.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

#d12.o: d12.c
#	$(CC) $(CFLAGS) --param max-inline-insns-auto=2 -c $< -o $@

test.o: tests/test.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# CONVERT ELF
%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.ee_srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

test.elf: test.o midi_link_desc.o usb_midi.o d12.o USBMidiUart.o MidiUart.o \
	  MidiClock.o mididuino.o Midi.o LCD.o GUI.o MidiTools.o WMath.o
	$(CC) $(CLDFLAGS) -o $@ $+

avrdude: clean $(PROJ).hex # $(PROJ).ee_srec $(PROJ).hex
	avrdude -p m32 -P usb -c usbasp -U flash:w:$(PROJ).hex

avrread: 
	avrdude -p m32 -P usb -c usbasp -U flash:r:$(PROJ)-read.hex:i

init:
#       enable watchdog, external crystal
# lfuse = 00100000 // external clock
# 4 kb bootloader
	avrdude -p m32 -P usb -c usbasp -U hfuse:w:0xd8:m -U lfuse:w:0x20:m

readfuses:
	avrdude -p m32 -P usb -c usbasp -U hfuse:r:high.raw -U lfuse:r:low.raw

restart:
#       read the fuses to reset the programming adapter
	avrdude -p m32 -P usb -c usbasp 

clean:
	rm -f *.o *.d *~ $(PROJ).elf *.elf *.srec *.ee_srec *.hex
