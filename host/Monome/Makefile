CXX ?= g++
CC ?= gcc
CFLAGS += -Os
CXXFLAGS += -Os
CLDCXX ?= g++
CLDC ?= gcc
# ARCHFLAGS = -arch i386 -arch ppc  -mmacosx-version-min=10.4

BASE_DIR=../..
LBASE_DIR=$(BASE_DIR)/hardware/libraries
JAR_DIR=$(BASE_DIR)/build/macosx/build/work/Arduino.app/Contents/Resources/Java
CLASSPATH=$(subst jar ,jar:,$(wildcard $(JAR_DIR)/*.jar))

DIRS  = CommonTools Midi Monome
LDIRS = $(foreach dir,$(DIRS),$(LBASE_DIR)/$(dir)) ../MidiUart ../GUI ../Tools
INCS  = $(foreach dir,$(LDIRS),-I$(dir)) -I.. -I../Tools -I/opt/local/include
EXCLUDE =  %MDArpeggiator.cpp
OBJS  = $(foreach dir,$(LDIRS),$(foreach file,$(filter-out $(EXCLUDE),$(wildcard $(dir)/*.cpp)),$(subst .cpp,.host.o,$(file))))
OBJS += $(foreach dir,$(LDIRS),$(foreach file,$(wildcard $(dir)/*.c),$(subst .c,.host.o,$(file))))

CXXFLAGS += -Wall  -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g
CFLAGS += -Wall -DHOST_MIDIDUINO -I. -ffunction-sections $(ARCHFLAGS) -g	
#CLDFLAGS += -lreadline -L/opt/local/lib
CLDFLAGS += --gc-sections

UNAME=$(shell uname)
ISWIN=$(findstring CYGWIN,$(UNAME))
ISMAC=$(findstring Darwin,$(UNAME))

ifneq (,$(ISMAC))
CFLAGS += -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5
CXXFLAGS += -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5
CLDFLAGS +=  -framework CoreAudio -framework CoreMIDI -framework Carbon $(ARCHFLAGS)
CLDFLAGS += -isysroot /Developer/SDKs/MacOSX10.5.sdk -mmacosx-version-min=10.5
endif

ifneq (,$(ISWIN))
LIBS += -lwinmm
endif


# MACOSX FLAGS

CXXFLAGS += $(INCS)
CFLAGS += $(INCS)

OBJS += 

all: monometest monometest2

%.host.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.host.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

monometest: $(OBJS) MonomeTrigPage.o MonomeHost.o MonomeMidiPage.o monometest.host.o
	$(CLDCXX) $(CLDFLAGS) -o $@ $^ $(LIBS)

monometest2: $(OBJS) MonomeHost.o monometest2.host.o
	$(CLDCXX) $(CLDFLAGS) -o $@ $^ $(LIBS)


clean:
	- rm -f *.o monometest

libclean:
	rm -rf $(OBJS)

