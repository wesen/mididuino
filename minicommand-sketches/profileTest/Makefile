PDEDIR ?= .

CC=/Applications/mididuino-0013/hardware/tools/avr/bin/avr-gcc
CXX=/Applications/mididuino-0013/hardware/tools/avr/bin/avr-g++
OBJCOPY =/Applications/mididuino-0013/hardware/tools/avr/bin/avr-objcopy
UISP=uisp
AVR_ARCH = atmega64
F_CPU = 16000000L

CORE = minicommand2
BASE_DIR = /Users/manuel/mididuino
LBASE_DIR = $(BASE_DIR)/hardware/libraries
JAR_DIR = $(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH = $(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

PDEFILES=$(wildcard $(PDEDIR)/*.pde)

#CFLAGS += -Os -g -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(AVR_ARCH) -DF_CPU=$(F_CPU)
#CFLAGS += -Wl,--section-start=.bss=0x802000,--defsym=__heap_end=0x80ffff -Wall -Werror
#CLDFLAGS += -Os -Wl,--gc-sections -Wl,--section-start=.bss=0x802000,--defsym=__heap_end=0x80ffff
#CLDFLAGS += -mmcu=$(AVR_ARCH)

OPTIM=-Os
#OPTIM=

CFLAGS = -c -g $(OPTIM) -w -ffunction-sections -fdata-sections -mmcu=atmega64 -DF_CPU=16000000L -Wl,--section-start=.bss=0x802000,--defsym=__heap_end=0x80ffff
CXXFLAGS = -c -g $(OPTIM) -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=atmega64 -DF_CPU=16000000L -Wl,--section-start=.bss=0x802000,--defsym=__heap_end=0x80ffff
CLDFLAGS =  $(OPTIM) -Wl,--gc-sections -mmcu=atmega64 -o /Users/manuel/mididuino/minicommand-sketches/profileTest/applet/profileTest.elf -Wl,--section-start=.bss=0x802000,--defsym=__heap_end=0x80ffff

#DIRS = CommonTools Midi GUI SDCard MNM MD MidiTools Elektron
DIRS = CommonTools Midi GUI Elektron
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) $(BASE_DIR)/hardware/cores/$(CORE)
INCS = $(foreach dir,$(LDIRS),-I$(dir))
OBJS = $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.cpp),$(subst .cpp,.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.o,$(file))))

CFLAGS += $(INCS)
CXXFLAGS += $(INCS)

all: main.hex

main.cpp: $(PDEFILES) $(wildcard $(BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	echo $(wildcard $(BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	java -cp $(CLASSPATH) processing.app.preproc.PdePreprocessor ./main $(PDEFILES)

main.elf: main.o $(OBJS)
	$(CC) $(CLDFLAGS) -o $@ $^ -lm

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.s Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.s: %.cpp
	$(CXX) -S $(CXXFLAGS) -fverbose-asm $< -o $@

%.os: %.o
	avr-objdump -S $< > $@

%.elfs: %.elf
	avr-objdump -S $< > $@

%.o: %.S
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

%.d:%.c
	set -e; $(CC) -MM $(CFLAGS) $< \
	| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@ ; \
	[ -s $@ ] || rm -f $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.ee_srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@


libclean:
	rm -rf $(OBJS)

clean:
	rm -rf *.os *.o *.elf *.elfs *.lst main.cpp