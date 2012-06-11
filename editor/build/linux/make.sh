#!/bin/sh

# This file is subjected to the GPL License

# NOTE: before running this script, you must set CLASSPATH to
# your standard Java classpath, and DIRAVR to the root of your
# avr-gcc installation.

DISTNAME=MidiCtrl
HARDWAREDIR=../../hardware
DISTDIR=../../../mididuino-dist
SHAREDDIST=${DISTDIR}/shared
RESOURCES=`pwd`/work

### -- SETUP WORK DIR -------------------------------------------

if test -d work 
then
  BUILD_PREPROC=false
else
  BUILD_PREPROC=true

  # needs to make the dir because of packaging goofiness
  echo Setting up directories to build under Linux
  mkdir -p work/classes/processing/app/preproc
  mkdir -p work/classes/processing/app/syntax
  mkdir -p work/classes/processing/app/tools
  mkdir -p work/lib/build

	cp ../../readme.txt work
	cp ../../license.txt work
	cp ../../building.txt work

	cp -r ../../app/lib "$RESOURCES/"

  cp dist/midi-ctrl work/
fi

# Build midi-send tool
cd ../../hardware/tools/mididuino
make -f Makefile.linux
cd ../../../build/linux

echo Copying shared and core files...
cp -r ${SHAREDDIST}/* work
rm -rf work/dist
cp -r ../../hardware/tools "$RESOURCES/"
# cp ../../hardware/tools/avr-extras/avr5.x "$RESOURCES/hardware/avr/avr/lib/ldscripts"

cp -r "$HARDWAREDIR" "$RESOURCES/"
cp ../../app/lib/antlr.jar "$RESOURCES/lib"
cp ../../app/lib/jna.jar "$RESOURCES/lib"
cp ../../app/lib/oro.jar "$RESOURCES/lib"

echo Copying examples...
mkdir "$RESOURCES/examples"
cp -r ../../minicommand-sketches/PublishedFirmwares "$RESOURCES/examples/"
cp -r ../../minicommand-sketches/Tutorials "$RESOURCES/examples/"

### -- START BUILDING -------------------------------------------

# move to root 'arduino' directory
cd ../..

### -- BUILD CORE ------------------------------------------------

echo Building processing.core...

cd core

#CLASSPATH=/System/Library/Frameworks/JavaVM.framework/Classes/classes.jar:/System/Library/Frameworks/JavaVM.framework/Classes/ui.jar:/System/Library/Java/Extensions/QTJava.zip
#export CLASSPATH

perl preproc.pl

mkdir -p bin
javac -source 1.5 -target 1.5 -d bin \
  src/processing/core/*.java \
  src/processing/xml/*.java

rm -f "$RESOURCES/lib/core.jar"

cd bin && \
		zip -rq "$RESOURCES/lib/core.jar" \
  processing/core/*.class \
  processing/xml/*.class
cd ..

# head back to "processing/app"
echo `pwd`
cd ../app 
echo `pwd`



### -- BUILD PDE ------------------------------------------------

echo Building the PDE...

# For some reason, javac really wants this folder to exist beforehand.
rm -rf ../build/linux/work/classes
mkdir ../build/linux/work/classes
# Intentionally keeping this separate from the 'bin' folder
# used by eclipse so that they don't cause conflicts.

javac \
    -Xlint:deprecation \
    -source 1.5 -target 1.5 \
    -classpath "$CLASSPATH:$RESOURCES/lib/core.jar:$RESOURCES/lib/antlr.jar:$RESOURCES/lib/ecj.jar:$RESOURCES/lib/jna.jar:$RESOURCES/lib/oro.jar" \
    -d ../build/linux/work/classes \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/linux/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/tools/*.java \
    src/processing/app/library/*.java

cd ../build/linux/work/classes
rm -f "$RESOURCES/lib/pde.jar"
zip -0rq "$RESOURCES/lib/pde.jar" .
cd ../..

rm -rf "$RESOURCES/classes"
# get updated core.jar and pde.jar; also antlr.jar and others
#mkdir -p work/${DISTNAME}.app/Contents/Resources/Java/
#cp work/lib/*.jar work/${DISTNAME}.app/Contents/Resources/Java/


echo
echo Done.
