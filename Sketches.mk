#
# MidiCtrl - Makefile for midi-ctrl sketches
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

UNAME=$(shell uname)
ISWIN=$(findstring CYGWIN,$(UNAME))
ISMAC=$(findstring Darwin,$(UNAME))

ifneq (,$(ISMAC))
MIDICTRL_BASE_DIR=/Applications/MidiCtrl.app/Contents/Resources/Java
endif

ifneq (,$(ISWIN))
MIDICTRL_BASE_DIR = C:/Program Files/MidiCtrl
endif

JAR_DIR = $(MIDICTRL_BASE_DIR)
CLASSPATH = $(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))
JAVA_FLAGS += -Duser.dir="$(MIDICTRL_BASE_DIR)" -cp "$(CLASSPATH)"

PDEDIR ?= .
CURRENT_DIR := $(shell pwd)
PDENAME := $(notdir $(CURRENT_DIR))
PDEFILES=$(wildcard $(PDEDIR)/*.pde)

# settings for minicommand2
CC=avr-gcc
CXX=avr-g++
OBJCOPY = avr-objcopy
UISP=uisp
AVR_ARCH = atmega64
F_CPU = 16000000L
CORE = minicommand2

MIDICTRL_LIBS += GUI

#LDIRS := $(shell java -cp "$(CLASSPATH)" processing.app.debug.Compiler --dir "$(BASE_DIR)" --board $(CORE) --libraries `pwd`/$(TARGET).pde)
#LDIRS += $(CORE_DIR)
#INCS = $(foreach dir,$(LDIRS),-I$(dir))

# read out compile flags from the actual MidiCtrl app
CFLAGS = $(shell cd $(MIDICTRL_BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-c-flags)
CXXFLAGS = $(shell cd $(MIDICTRL_BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-cxx-flags)
CLDFLAGS = $(shell cd $(MIDICTRL_BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-ld-flags)

# include general midi settings
CURDIR := $(dir $(lastword $(MAKEFILE_LIST)))
include $(CURDIR)MidiCtrl.mk

all: $(PDENAME).hex

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s 
	$(CC) $(CFLAGS) -c $< -o $@

%.s: %.c
	$(CC) -S $(CFLAGS) -fverbose-asm $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

%.syx: %.hex
	ihex2sysex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.ee_srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@

testflags:	
	cd $(MIDICTRL_BASE_DIR) && java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --print-c-flags

$(PDENAME).o: $(PDEFILES) $(wildcard $(MIDICTRL_BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	java $(JAVA_FLAGS) processing.app.preproc.PdePreprocessor ./$(PDENAME) $(PDEFILES)
	$(CXX) $(CXXFLAGS) -c $(PDENAME).cpp -o $@
	rm $(PDENAME).cpp

upload: $(PDENAME).hex
	midi-send -I 0x41 -b -i${MIDI_INPUT_PORT} -o${MIDI_OUTPUT_PORT} $<

$(PDENAME)whole.elf: $(PDENAME).cpp $(COBJS)
	$(CXX) $(CLDFLAGS) $(CFLAGS) -o $@ -fwhole-program --combine $^ $(SRCS)

$(PDENAME).elf: $(PDENAME).o $(OBJS)
	$(CXX) $(CLDFLAGS) -g -o $@ $^ 

