#!/bin/sh

rm -rf tmp

find ../../hardware -name \*.o | xargs rm

mkdir tmp
cp -r ../../hardware/arduino-libraries/MidiUart tmp
for i in LFO MD Midi MidiTools Sequencer Commontools; do
    cp -r ../../hardware/libraries/$i tmp
done
find tmp -name .svn | xargs rm -rf
cp -r ../../hardware/arduino-libraries/README.txt tmp

cd tmp
zip -r MidiDuino-arduino.zip *
mv MidiDuino-arduino.zip ..
cd ..

rm -rf tmp
mkdir tmp
cp -r ../../hardware/cores/arduinomidi tmp
cp -r ../../hardware/arduino-libraries/boards.txt tmp
cp -r ../../hardware/arduino-libraries/README-core.txt tmp
find tmp -name .svn | xargs rm -rf
cd tmp
zip -r MidiDuino-arduinomidi-core.zip *
mv MidiDuino-arduinomidi-core.zip ..
cd ..

rm -rf tmp

mkdir tmp
cp -r ../../minicommand-sketches/ArduinoMidi* tmp
find tmp -name .svn  | xargs rm -rf
cd tmp
zip -r MidiDuino-examples.zip *
mv MidiDuino-examples.zip ..
cd ..

rm -rf tmp