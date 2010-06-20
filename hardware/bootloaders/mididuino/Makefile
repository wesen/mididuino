CC=avr-gcc
OBJCOPY = avr-objcopy
UISP=uisp
#AVR_ARCH = atmega8
#LDAVR_ARCH = avrmega8
AVR_ARCH = atmega168
LDAVR_ARCH = avrmega168
#CFLAGS += -DENABLE_MENU -DENABLE_MACRO
#CFLAGS += -DENABLE_MERGE -DENABLE_MENU
#CFLAGS += -DENABLE_MENU
#CFLAGS += -DENABLE_MIDI_CLOCK -DENABLE_MIDI_LEARN -DENABLE_MENU
CFLAGS += -DENABLE_MENU -DENABLE_MIDI_CLOCK -DENABLE_MIDI_LEARN
FLASH_PROTOCOL = jtag2isp

CFLAGS += -Os -ffunction-sections -DAVR -I. -mmcu=$(AVR_ARCH) -mcall-prologues -fshort-enums -fpack-struct -Wall -Werror
CFLAGS += -Wall -DLITTLE_ENDIAN -g
CLDFLAGS += -Wl,--gc-sections -ffunction-sections
CLDFLAGS += -mmcu=$(AVR_ARCH)
LDFLAGS = -m $(LDAVR_ARCH) -M

PROJ=midi-kontrol

# atmega 8 512 word boot size = 0x1c00 
BOOTLOADER8_START = 0x1C00
# atmega 1688 512 word boot size = 
BOOTLOADER168_START = 0x3800

#CLDFLAGS += -Wl,--section-start=.bootstart=$(BOOTLOADER_START)
#CLDFLAGS += -Wl,--section-start=.bootloader=0x1820
#CLDFLAGS += -Wl,--section-start=.bootloader.formidable=0x0880
#CLDFLAGS += -Wl,--section-start=.data=0x800

all: bootloader168.hex

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

bootloader168.elf: bootloader.o lcd.o
	$(CC) $(CLDFLAGS) -Wl,--section-start=.text=$(BOOTLOADER168_START) -o $@ $+

bootloader.elf: bootloader.o 
	$(CC) $(CLDFLAGS) -Wl,--section-start=.text=$(BOOTLOADER8_START) -o $@ $+

bootloader-asm.elf: bootloader-asm.o 
	$(CC) $(CLDFLAGS) -Wl,--section-start=.text=$(BOOTLOADER8_START) -o $@ $+
#	$(CC) $(CLDFLAGS) -o $@ $+

# PROGRAM
bootavrdude: bootloader.hex # $(PROJ).ee_srec $(PROJ).hex
	avrdude -p m8 -P usb -c $(FLASH_PROTOCOL) -U flash:w:bootloader.hex

bootavrdude168: bootloader168.hex # $(PROJ).ee_srec $(PROJ).hex
	avrdude -p m168 -P usb -c $(FLASH_PROTOCOL) -U flash:w:bootloader168.hex -B 4


init512:
# 512 words bootloader
	avrdude -p m8 -P usb -c $(FLASH_PROTOCOL) -U hfuse:w:0xda:m -U lfuse:w:0x0f:m

init168_1024:
# 1024 words bootloader
	avrdude -p m168 -P usb -c $(FLASH_PROTOCOL) -U efuse:w:0x00:m -U hfuse:w:0xdc:m -U lfuse:w:0xee:m -B 4

restart:
#       read the fuses to reset the programming adapter
#	$(UISP) $(UISP_TARGET) --rd_fuses
	avrdude -p m8 -P usb -c $(FLASH_PROTOCOL) 

restart168:
#       read the fuses to reset the programming adapter
#	$(UISP) $(UISP_TARGET) --rd_fuses
	avrdude -p m168 -P usb -c $(FLASH_PROTOCOL) 

clean:
	rm -f *.o *.d *~ $(PROJ) *.srec *.ee_srec *.hex *lst *elf *syx
