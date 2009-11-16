#CXX = llvm-g++-4.2
CXX = g++
#CC = llvm-gcc-4.2
CC = gcc
#ARCHFLAGS = -arch x86_64

BASE_DIR=../..
LBASE_DIR=$(BASE_DIR)/hardware/libraries
JAR_DIR=$(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

DIRS  = CommonTools Midi MidiTools GUI MD Elektron MNM Sequencer
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) ../../host/MidiUart ../../host/GUI
INCS  = $(foreach dir,$(LDIRS),-I$(dir)) -I../../host
#EXCLUDE += %MidiClock.cpp
OBJS  = $(foreach dir,$(LDIRS),$(foreach file,$(filter-out $(EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.host.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.host.o,$(file))))

CPPUNITDIR=$(BASE_DIR)/thirdparty/CppUnitLite2
CXXFLAGS += -I$(CPPUNITDIR)/src
CLDFLAGS += $(CPPUNITDIR)/cppunitlite.a

CXXFLAGS += -Wall  -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g
CFLAGS += -Os
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

all: test

%.host.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.host.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

test: main.host.o bitTests.host.o listTest.host.o \
      arpTest.host.o stackTest.host.o miscTest.host.o \
      midiTest.host.o callbackTest.host.o noteHandlerTest.host.o \
      mdPatternTest.host.o mnmPatternTest.host.o dataEncoderTest.host.o \
			mnmDataEncoderTest.host.o \
      $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^ $(CPPUNITDIR)/cppunitlite.a $(LIBS)

clean:
	- rm -f *.o test

libclean:
	rm -rf $(OBJS)

