PDEDIR ?= .

CC=avr-gcc
CXX=avr-g++
LLVMCC=avr-llvm-gcc
LLVMCXX=avr-llvm-g++
OBJCOPY = avr-objcopy
UISP=uisp
AVR_ARCH = atmega64
F_CPU = 16000000L

CORE = minicommand2
BASE_DIR = /Applications
# LBASE_DIR = $(BASE_DIR)/hardware/libraries
LBASE_DIR = /Users/manuel/code/wesen/mididuino/hardware/libraries
JAR_DIR = $(BASE_DIR)/MidiCtrl.app/Contents/Resources/Java
CLASSPATH = $(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

PDEFILES=$(wildcard $(PDEDIR)/*.pde)

CFLAGS += -Os -w -fno-exceptions -ffunction-sections -fdata-sections -mmcu=$(AVR_ARCH) -DF_CPU=$(F_CPU)
CFLAGS += -Wl,--section-start=.bss=0x802000,--defsym=__heap_end=0x80ffff
CFLAGS += -mcall-prologues -funsigned-char -funsigned-bitfields -fshort-enums -fpack-struct -Wall -Werror 
CFLAGS += -fno-default-inline -nostdinc++ -fno-rtti -fno-implement-inlines 

#CFLAGS += --param inline-call-cost=2 -finline-limit=3 -fno-inline-small-functions
#CFLAGS += -fno-tree-scev-cprop -fsplit-wide-types

#CFLAGS += -fno-inline
CLDFLAGS += -Wl,--relax,--gc-sections -ffunction-sections -g
CLDFLAGS += -mmcu=$(AVR_ARCH)

DIRS = CommonTools Midi GUI SDCard MNM MD MidiTools Elektron PageTools
LDIRS = $(wildcard $(LBASE_DIR)/*) $(BASE_DIR)/hardware/cores/$(CORE)
INCS = $(foreach dir,$(LDIRS),-I$(dir))

SRCS =  $(foreach dir,$(LDIRS),$(wildcard $(dir)/*.cpp))
#SRCS += $(foreach dir,$(LDIRS),$(wildcard $(dir)/*.c))

OBJS = $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.cpp),$(subst .cpp,.o,$(file))))
COBJS = $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.o,$(file))))
LLVMOBJS = $(foreach obj,$(OBJS),$(subst .o,.llvm.o,$(obj)))

CFLAGS += $(INCS)
LLVMCFLAGS ?= -emit-llvm -O4 $(CFLAGS)

all: main.hex

main.cpp: $(PDEFILES) $(wildcard $(BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	echo $(wildcard $(BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	java -Duser.dir=$(BASE_DIR)/MidiCtrl.app/Contents/Resources/Java -cp "$(CLASSPATH)" processing.app.preproc.PdePreprocessor ./main $(PDEFILES)

mainwhole.elf: main.cpp $(COBJS)
	$(CXX) $(CLDFLAGS) $(CFLAGS) -o $@ -fwhole-program --combine $^ $(SRCS)

main.bc.o: main.llvm.o $(LLVMOBJS)
	llvm-link -o $@ -f $^

main.elf: main.o $(OBJS)
	$(CXX) $(CLDFLAGS) -g -o $@ $^ 

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp Makefile
	$(CXX) $(CFLAGS) -c $< -o $@

%.llvm.o: %.c Makefile
	$(LLVMCC) $(LLVMCFLAGS) -emit-llvm -c $< -o $@

%.llvm.o: %.cpp Makefile
	$(LLVMCXX) $(LLVMCFLAGS) -emit-llvm -c $< -o $@

%.o: %.s Makefile
	$(CC) $(CFLAGS) -c $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.s: %.cpp
	$(CXX) -S $(CFLAGS) -fverbose-asm $< -o $@

%.os: %.o
	avr-objdump -S $< > $@

%.elfs: %.elf
	avr-objdump -S $< > $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

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
