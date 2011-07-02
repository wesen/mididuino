#CXX = llvm-g++-4.2
CXX = g++
#CC = llvm-gcc-4.2
CC = gcc
#ARCHFLAGS = -arch x86_64

BASE_DIR=../..
LBASE_DIR=$(BASE_DIR)/hardware/libraries
JAR_DIR=$(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

DIRS  = CommonTools Midi MidiTools GUI Elektron MD MNM Sequencer
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) ../../host/MidiUart ../../host/GUI
INCS  = $(foreach dir,$(LDIRS),-I$(dir)) -I../../host
#EXCLUDE += %MidiClock.cpp
OBJS  = $(foreach dir,$(LDIRS),$(foreach file,$(filter-out $(EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.host.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.host.o,$(file))))

CPPUNITDIR=$(BASE_DIR)/thirdparty/CppUnitLite2
CXXFLAGS += -I$(CPPUNITDIR)/src
CLDFLAGS += $(CPPUNITDIR)/cppunitlite.a

CXXFLAGS += -Wall  -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g -O4
CFLAGS += -O2
CFLAGS += -Wall -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g

UNAME=$(shell uname)
ISWIN=$(findstring CYGWIN,$(UNAME))
ISMAC=$(findstring Darwin,$(UNAME))

ifneq (,$(ISMAC))
CLDFLAGS +=  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)
endif

ifneq (,$(ISWIN))
LIBS += -lwinmm
endif

CXXFLAGS += $(INCS)
CFLAGS += $(INCS)

CPPSOURCES := $(shell find . -name '*.cpp')
OBJFILES = $(foreach file,$(CPPSOURCES),$(subst .cpp,.o,$(file)))
HOSTOBJFILES = $(foreach file,$(CPPSOURCES),$(subst .cpp,.host.o,$(file)))

all: test

%.host.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.host.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: $(HOSTOBJFILES) $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

clean:
	- rm -f *.o test
	- find . -name \*.o -print0 | xargs -0 rm

libclean:
	rm -rf $(OBJS)

