CXX = g++
CC = gcc

BASE_DIR=../..
LBASE_DIR=$(BASE_DIR)/hardware/libraries
JAR_DIR=$(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

DIRS  = GUI CommonTools Midi MidiTools MNM Elektron Sequencer
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) ../../host/MidiUart ../../host/GUI
INCS  = $(foreach dir,$(LDIRS),-I$(dir)) -I../../host
EXCLUDE = %MDArpeggiator.cpp
OBJS  = $(foreach dir,$(LDIRS),$(foreach file,$(filter-out $(EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.host.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.host.o,$(file))))

CXXFLAGS += -Wall  -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g
CFLAGS += -Wall -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g
CLDFLAGS =  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)

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

all: mnmPatternTest

%.host.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.host.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

mnmTest: mnmTest.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^

mnmPatternTest: mnmPatternTest.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^


clean:
	- rm -f *.o mnmTest mnmPatternTest *.host.o

libclean:
	rm -rf $(OBJS)

