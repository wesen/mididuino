include ../../../AVR.mk

PROJ=bootloader

BOOTLOADER64_START = 0xF000

all: bootloader.hex

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
