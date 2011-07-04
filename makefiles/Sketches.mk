#
# MidiCtrl - Makefile for midi-ctrl sketches
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

# get the current OS
UNAME=$(shell uname)
ISWIN=$(findstring CYGWIN,$(UNAME))
ISMAC=$(findstring Darwin,$(UNAME))

CURDIR := $(dir $(lastword $(MAKEFILE_LIST)))
MIDICTRL_BASE_DIR  ?= $(abspath $(CURDIR)..)

ifneq (,$(ISMAC))
# default location of application folder
APP_DIR ?= /Applications/MidiCtrl.app/Contents/Resources/Java
endif

ifneq (,$(ISWIN))
APP_DIR ?= C:/Program Files/MidiCtrl
endif

# prepare the java settings
JAR_DIR := $(APP_DIR)
CLASSPATH := $(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))
JAVA_FLAGS := -Duser.dir="$(MIDICTRL_BASE_DIR)" -cp "$(CLASSPATH)"

# get the current sketch name
PDEDIR ?= .
CURRENT_DIR := $(shell pwd)
PDENAME := $(notdir $(CURRENT_DIR))
PDEFILES=$(wildcard $(PDEDIR)/*.pde)

# settings for minicommand2
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
AVR_ARCH = atmega64
F_CPU = 16000000L
CORE = minicommand2

# add the core directory to the include path
MIDICTRL_LIB_DIRS += $(MIDICTRL_BASE_DIR)/hardware/cores/$(CORE)

# Read out the libraries from the midi ctrl environment
.midictrl.flags: $(PDEFILES)
	@java $(JAVA_FLAGS) processing.app.debug.Compiler --board $(CORE) --make $(CURRENT_DIR)/$(PDENAME).pde > $@

include .midictrl.flags
include $(CURDIR)MidiCtrl.mk

all: $(PDENAME).hex

printinfo:
	@echo "MidiCtrl base dir: " $(MIDICTRL_BASE_DIR)
	@echo "Application dir: " $(APP_DIR)
	@echo "JAR dir: " $(JAR_DIR)
	@echo "Classpath: " $(CLASSPATH)
	@echo "CFLAGS: " $(CFLAGS)
	@echo "CXXFLAGS: " $(CXXFLAGS)
	@echo "CLDFLAGS: " $(CLDFLAGS)
	@echo "Libraries: " $(MIDICTRL_LIBS)

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

# build the PDE by calling the preprocessor
$(PDENAME).o: $(PDEFILES) $(wildcard $(MIDICTRL_BASE_DIR)/hardware/cores/$(CORE)/*.cxx)
	@java $(JAVA_FLAGS) processing.app.preproc.PdePreprocessor ./$(PDENAME) $(PDEFILES) > /dev/null
	$(CXX) $(CXXFLAGS) -c $(PDENAME).cpp -o $@
	rm $(PDENAME).cpp

upload: $(PDENAME).hex
	midi-send -I 0x41 -b -i${MIDI_INPUT_PORT} -o${MIDI_OUTPUT_PORT} $<

$(PDENAME)whole.elf: $(PDENAME).cpp $(COBJS)
	$(CXX) $(CLDFLAGS) $(CFLAGS) -o $@ -fwhole-program --combine $^ $(SRCS)

$(PDENAME).elf: $(PDENAME).o $(MIDICTRL_OBJS)
	$(CXX) $(CLDFLAGS) -g -o $@ $^ 

clean:
	- rm *.elf *.hex *.o .midictrl.flags

libclean:
	rm -rf $(MIDICTRL_HOST_OBJS) $(HOST_OBJS)
