MidiDuino Library: MIDI for the Arduino
-------------------------------

Check http://ruinwesen.com/ for documentation, new library versions,
tutorials and other interesting MIDI information.

Installation
---------

Copy the folders CommonTools, LFO, MD, Midi, MidiTools, MidiUart and Sequencer
into the hardware/libraries folder of your Arduino installation. 


Description
---------

CommonTools is a collection of useful macros, functions and classes
that are used by other parts of the MidiDuino library.

Midi is the actual Midi Stack implementation that takes care of
parsing incoming MIDI data, calling callback functions and handling
MIDI clock synchronization.

MidiUart takes care of sending MIDI data, and of setting the serial
interface to the right speed. It implements a buffered sending of the
serial data, which is not done in the normal serial
implementation. The MidiUart only works for the Atmega168 and
Atmega328 based Arduinos for now, a port to the Arduino Mega is in preparation.

MidiTools provides rudimentary tools to modify MIDI data.

LFO provides a clock synchronized implementation of LFOs, which is
very rudimentary for now as well.

Sequencer provides some basic sequencer implementation, which are
quite rudimentary for now as well.

MD provides a big implementation of functions to interface with the
Elektron MachineDrum. The implementation takes quite a lot of static
ram, so that it is not useful on small Arduino like the Duemilanove at
the moment.


Usage
-----

Please refer to the MidiDuino library manual at
http://ruinwesen.com/mididuino and the MidiDuino library reference at
the same address.


Example sketches
--------------

The library includes a number of example sketches that you can
download at http://ruinwesen.com/mididuino to get started in your MIDI
exploration.


FAQ
---

Q: What's the MiniCommand and the MidiDuino software environment?

A: All the software included in the MidiDuino library was originally
written for the Ruin & Wesen MIDI controllers, the first one available
being the MiniCommand. These controllers offer a complete hardware
MIDI Controller development environment, and use a custom version of
the Arduino environment for programming. The MIDI code written for
these devices was then ported over to the standard Arduino.

The development environment used to program the MiniCommand is also
called MidiDuino, in contrast to the MidiDuino library, which is what
you have just downloaded. Check out the MiniCommand at
http://ruinwesen.com/digital .


Q: Can I send MIDI directly over the USB serial cable?

A: Yes, but you have to use a program to convert the serial data to
MIDI data. On linux, you have MidiBang
(http://code.google.com/p/midibang/). Under MacOSX, you can use the
ardrumo software (http://code.google.com/p/ardrumo/) or Serial MIDI
Converter
(http://www.spikenzielabs.com/SpikenzieLabs/Serial_MIDI.html).  Under
Windows, you can use s2midi (http://www.memeteam.net/2007/s2midi/).
Be sure to disconnect these programs before uploading a new sketch to
the Arduino, as they will interfere.


Q: Why does my Arduino time out when programming?

A: Check if the RX wire is connected to the input MIDI
circuit. Disconnect it before programming.


Q: Why is setTempo() not doing the right thing?

A: We are aware of that and will fix the correct tempo settings in a
future version.


Q: Why does my sketch not seem to work when I use the MD library?

A: That's because MD uses too much RAM for the 1kB of the Arduino
Atmega328. Wait for a future release to be able to use the MD
functionality.


Q: Why can't I send MIDI in the setup() function?

A: You have to enable interrupts before sending data on the serial
interface, as it uses a buffered output. If you want to
send MIDI from the setup(), call sei() before sending the MIDI
data. Be sure to do this at the end of the setup() routine. 
