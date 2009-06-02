MidiDuino core replacement for Arduino
--------------------------------

Check http://ruinwesen.com/ for documentation, new library versions,
tutorials and other interesting MIDI information.

Installation
---------

Copy the folder arduinomidi into the hardware/cores folder of your
Arduino installation. Then copy the content of the boards.txt to the
end of the hardware/boards.txt in you Arduino installation, and
restart the Arduino installation. The entries “Arduino Duemilanove w/
Atmega328 and MIDI” and “Arduino Diecimila or Duemilanove w/ Atmega168
and MIDI” should now be available in the Tools -> Boards menu of the
Arduino environment.

You still need to install the MidiDuino libraries separatly. You can
download them from http://ruinwesen.com/mididuino.

Description
---------

This is a replacement core for arduino that will transmit MIDI clock
bytes received on the serial interface directly to the MIDI stack. 
