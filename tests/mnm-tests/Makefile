CXX = g++
CC = gcc
ARCHFLAGS = -arch i386 -arch ppc  -mmacosx-version-min=10.4

BASE_DIR=../..
LBASE_DIR=$(BASE_DIR)/hardware/libraries
JAR_DIR=$(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

DIRS  = GUI CommonTools Midi MidiTools MNM Elektron
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) ../MidiUart ../GUI
INCS  = $(foreach dir,$(LDIRS),-I$(dir)) -I..
EXCLUDE = %MidiClock.cpp %ModalGui.cpp %MDArpeggiator.cpp
OBJS  = $(foreach dir,$(LDIRS),$(foreach file,$(filter-out $(EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.host.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.host.o,$(file))))

CXXFLAGS += -Wall  -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS)
CFLAGS += -Wall -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS)
CLDFLAGS =  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)

CXXFLAGS += $(INCS)
CFLAGS += $(INCS)

all: mnmTest

%.host.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.host.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

mnmTest: mnmTest.host.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^


clean:
	- rm -f *.o profiler merger

libclean:
	rm -rf $(OBJS)

