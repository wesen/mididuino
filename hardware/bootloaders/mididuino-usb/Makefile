CC=avr-gcc
OBJCOPY = avr-objcopy
UISP=uisp
AVR_ARCH = atmega32
LDAVR_ARCH = avrmega32
CFLAGS += -Os -ffunction-sections -DAVR -I. -mmcu=$(AVR_ARCH) -frtl-abstract-sequences
CFLAGS += -Wall -DLITTLE_ENDIAN -Werror -Winline --param max-inline-insns-auto=2
CLDFLAGS += -Wl,--gc-sections -mmcu=$(AVR_ARCH)
LDFLAGS = -m $(LDAVR_ARCH) -M
FLASH_PROTOCOL = usbasp

PROJ=mididuino-usb

all: $(PROJ).hex

# atmega 8 512 word boot size = 0x1c00 
BOOTLOADER8_START = 0x1C00
# atmega 1688 512 word boot size = 
BOOTLOADER168_START = 0x3800
# mega32 2048 word boot size
BOOTLOADER32_START = 0x7000

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

d12_boot.o: d12.c
	$(CC) $(CFLAGS) -DBOOTLOADER -c $< -o $@

d12_boot.s: d12.c
	$(CC) $(CFLAGS) -DBOOTLOADER -fverbose-asm -S $< -o $@


# CONVERT ELF
%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.ee_srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

test.elf: test.o lcd.o common.o
	$(CC) $(CLDFLAGS) -o $@ $+

pdiusb-midi-link.elf: pdiusb-midi-link.o midi_link_desc.o uart.o usb_midi.o d12.o lcd.o sr165.o common.o
	$(CC) $(CLDFLAGS) -o $@ $+

mididuino-usb.elf: bootloader-main.o midi_link_desc.o lcd.o d12_boot.o sr165.o usb_midi_bootloader.o bootloader.o common.o
	$(CC) $(CLDFLAGS) -Wl,--section-start=.text=$(BOOTLOADER32_START) -o $@ $+

avrdude: clean $(PROJ).hex # $(PROJ).ee_srec $(PROJ).hex
	avrdude -V -p m32 -P usb -c $(FLASH_PROTOCOL) -U flash:w:$(PROJ).hex

avrread: 
	avrdude -p m32 -P usb -c $(FLASH_PROTOCOL) -U flash:r:$(PROJ)-read.hex:i

init:
#       enable watchdog, external crystal
# lfuse = 00100000 // external clock
# 4 kb bootloader
	avrdude -p m32 -P usb -c $(FLASH_PROTOCOL) -U hfuse:w:0xd8:m -U lfuse:w:0x20:m

readfuses:
	avrdude -p m32 -P usb -c $(FLASH_PROTOCOL) -U hfuse:r:high.raw -U lfuse:r:low.raw

restart:
#       read the fuses to reset the programming adapter
	avrdude -p m32 -P usb -c $(FLASH_PROTOCOL) 

clean:
	rm -f *.o *.d *~ $(PROJ).elf *.elf *.srec *.ee_srec *.hex
