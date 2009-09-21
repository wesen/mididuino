CC      = avr-gcc
CXX     = avr-g++
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

ARCH  = atxmega128a1
F_CPU = 2000000L

CFLAGS += -Os
CFLAGS += -mmcu=$(ARCH) -DF_CPU=$(F_CPU)

CXXFLAGS += -Os
CXXFLAGS += -mmcu=$(ARCH) -DF_CPU=$(F_CPU)

all: xmega-test.hex

xmega-test.elf: xmega-test.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

program: xmega-test.hex
	$(AVRDUDE) -p x128a1 -c jtagmkii -P usb -U flash:w:$<:i

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	- rm -rf *hex *elf *.o