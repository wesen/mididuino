CXX = g++
CC = gcc
ARCHFLAGS = -arch i386 -arch ppc  -mmacosx-version-min=10.4

BASE_DIR=../..
LBASE_DIR=$(BASE_DIR)/hardware/libraries
JAR_DIR=$(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

DIRS  = CommonTools Midi Elektron
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) ../MidiUart
INCS  = $(foreach dir,$(LDIRS),-I$(dir)) -I..
EXCLUDE = %MidiClock.cpp
OBJS  = $(foreach dir,$(LDIRS),$(foreach file,$(filter-out $(EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.o,$(file))))

CXXFLAGS += -Wall  -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS)
CFLAGS += -Wall -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS)
CLDFLAGS =  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)

CXXFLAGS += $(INCS)
CFLAGS += $(INCS)

all: profiler

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

profiler: profiler.o $(OBJS)
	$(CXX) $(CLDFLAGS) -o $@ $^

clean:
	- rm -f *.o profiler

libclean:
	rm -rf $(OBJS)

