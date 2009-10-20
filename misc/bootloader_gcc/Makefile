CC      = avr-gcc
CXX     = avr-g++
OBJCOPY = avr-objcopy

ARCH  = atxmega128a1
F_CPU = 32000000L

BOOTLOADER_START = 0x20000

CFLAGS += -Os
CFLAGS += -mmcu=$(ARCH)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums

LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_START)

all: bootloader.hex

bootloader.elf: main.o eeprom_driver.o serial.o sp_driver.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.o: %.S
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	- rm -f *.o *.elf *.hex
