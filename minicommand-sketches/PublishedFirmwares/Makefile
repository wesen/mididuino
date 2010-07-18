MIDI_INPUT_PORT ?= 0
MIDI_OUTPUT_PORT ?= 0

PDEDIR ?= .
CURRENT_DIR := $(shell pwd)
PDENAME := $(notdir $(CURRENT_DIR))

CC=avr-gcc
CXX=avr-g++
OBJCOPY = avr-objcopy
UISP=uisp
AVR_ARCH = atmega64
F_CPU = 16000000L

CORE = minicommand2
BASE_DIR = /Applications/MidiCtrl.app/Contents/Resources/Java
#BASE_DIR = /Users/manuel/code/wesen/mididuino/build/macosx/work/MidiCtrl.app/Contents/Resources/Java
LBASE_DIR = $(BASE_DIR)/hardware/libraries
# LBASE_DIR = /Users/manuel/mididuino/hardware/libraries
JAR_DIR = $(BASE_DIR)
CLASSPATH = $(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

JAVA_FLAGS += -Duser.dir="$(BASE_DIR)" -cp "$(CLASSPATH)"

PDEFILES=$(wildcard $(PDEDIR)/*.pde)

CFLAGS = $(shell cd $(BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-c-flags)
CXXFLAGS = $(shell cd $(BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-cxx-flags)
CLDFLAGS = $(shell cd $(BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-ld-flags)

DIRS = CommonTools Midi GUI SDCard MNM MD MidiTools Elektron PageTools
LDIRS = $(wildcard $(LBASE_DIR)/*) $(BASE_DIR)/hardware/cores/$(CORE)
INCS = $(foreach dir,$(LDIRS),-I$(dir))

SRCS =  $(foreach dir,$(LDIRS),$(wildcard $(dir)/*.cpp))
#SRCS += $(foreach dir,$(LDIRS),$(wildcard $(dir)/*.c))

OBJS = $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.cpp),$(subst .cpp,.o,$(file))))
COBJS = $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.o,$(file))))

CFLAGS += $(INCS)
CXXFLAGS += $(INCS)

all: $(PDENAME).hex

testflags:	
	cd $(BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-c-flags

$(PDENAME).o: $(PDEFILES) $(wildcard $(BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	echo $(wildcard $(BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	java $(JAVA_FLAGS) processing.app.preproc.PdePreprocessor ./$(PDENAME) $(PDEFILES)
	$(CXX) $(CXXFLAGS) -c $(PDENAME).cpp -o $@
	rm $(PDENAME).cpp

upload: $(PDENAME).hex
	midi-send -I 0x41 -b -i${MIDI_INPUT_PORT} -o${MIDI_OUTPUT_PORT} $<

$(PDENAME)whole.elf: $(PDENAME).cpp $(COBJS)
	$(CXX) $(CLDFLAGS) $(CFLAGS) -o $@ -fwhole-program --combine $^ $(SRCS)

$(PDENAME).elf: $(PDENAME).o $(OBJS)
	$(CXX) $(CLDFLAGS) -g -o $@ $^ 

%.o: %.c Makefile
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

%.o: %.cpp Makefile
	$(CXX) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

%.o: %.s Makefile
	$(CC) $(CFLAGS) -Wa,-adhlns=$@.lst -c $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.s: %.cpp
	$(CXX) -S $(CFLAGS) -fverbose-asm $< -o $@

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
	rm -rf *.os *.o *.elf *.elfs *.lst $(PDENAME).cpp $(PDENAME).hex
