CC      = avr-gcc
CXX     = avr-g++
OBJCOPY = avr-objcopy

ARCH  = atxmega128a1
F_CPU = 32000000L

CFLAGS += -Os
CFLAGS += -mmcu=$(ARCH)

all: xmega-test.hex

xmega-test.elf: xmega-test.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
