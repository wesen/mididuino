CC=avr-gcc
OBJCOPY = avr-objcopy
AVR_ARCH = atmega64
LDAVR_ARCH = avrmega64
FLASH_PROTOCOL = jtag2

CFLAGS += -Os -ffunction-sections -DAVR -I. -mmcu=$(AVR_ARCH) -mcall-prologues -fshort-enums -fpack-struct -Wall -Werror
CFLAGS += -Wall -DLITTLE_ENDIAN -g
CLDFLAGS += -Wl,--gc-sections -ffunction-sections
CLDFLAGS += -mmcu=$(AVR_ARCH)
LDFLAGS = -m $(LDAVR_ARCH) -M

PROJ=bootloader

BOOTLOADER64_START = 0xF000

all: bootloader.hex

%.o: %.c Makefile
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@


%.o: %.s Makefile
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

%.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.syx: %.hex
	ihex2sysex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.ee_srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

bootloader.elf: bootloader.o lcd.o sr165.o
	$(CC) $(CLDFLAGS) -Wl,--section-start=.text=$(BOOTLOADER64_START) -o $@ $+

init64:
	avrdude -p m64 -P usb -c $(FLASH_PROTOCOL) -U efuse:w:0xff:m -U hfuse:w:0x8a:m -U lfuse:w:0x2e:m

#init64_2048:
#	avrdude -p m64 -P usb -c $(FLASH_PROTOCOL) -U efuse:w:0xff:m -U hfuse:w:0x8a:m -U lfuse:w:0x0f:m

bootavrdude64: bootloader.hex # $(PROJ).ee_srec $(PROJ).hex
	avrdude -p m64 -P usb -c $(FLASH_PROTOCOL) -V -U flash:w:bootloader.hex

restart64:
#       read the fuses to reset the programming adapter
#	$(UISP) $(UISP_TARGET) --rd_fuses
	avrdude -p m64 -P usb -c $(FLASH_PROTOCOL) 

avrread64: 
	avrdude -p m64 -P usb -c $(FLASH_PROTOCOL) -U flash:r:$(PROJ)-read.hex:i


clean:
	rm -f *.o *.d *~ $(PROJ) *.srec *.ee_srec *.hex *lst *elf *syx
