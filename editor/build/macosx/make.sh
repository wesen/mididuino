#!/bin/sh

#
# MidiCtrl - Build script for the MidiCtrl editor
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

### -- SETUP WORK DIR -------------------------------------------

DISTNAME=MidiCtrl
RESOURCES=`pwd`/work/${DISTNAME}.app/Contents/Resources/Java

MIDICTRL_BASE_DIR=`pwd`/../../..
HARDWARE_DIR=${MIDICTRL_BASE_DIR}/hardware
EDITOR_DIR=${MIDICTRL_BASE_DIR}/editor
DIST_DIR=${MIDICTRL_BASE_DIR}/editor/mididuino-dist
TOOLS_ZIP=${DIST_DIR}/macosx/tools-universal.zip
SHARED_DIST=${DIST_DIR}/shared
BUILDIR=`pwd`
WORK_DIR=`pwd`/work

# check if the work directory has been populated
if test -d "${WORK_DIR}"
then
    BUILD_PREPROC=false
else
    echo Setting up directories to build under Mac OS X
    BUILD_PREPROC=true

    mkdir "${WORK_DIR}"

    # to have a copy of this guy around for messing with
    echo Copying application skeleton
    cp -pRX "${DIST_DIR}/macosx/Application.app" "work/${DISTNAME}.app"

    echo Building midi-send application
    PREVDIR=`pwd`
    cd "${HARDWARE_DIR}/tools/mididuino" && make -f Makefile.OSX
    cd "${PREVDIR}"
    
    # cvs doesn't seem to want to honor the +x bit 
    chmod +x "work/${DISTNAME}.app/Contents/MacOS/JavaApplicationStub"

    echo Copying informational files
    for i in readme.txt license.txt building.txt; do
       cp "${MIDICTRL_BASE_DIR}/$i"  work
    done
    cp -rX "${EDITOR_DIR}/app/lib" "$RESOURCES/"
    
    cp -rX "$HARDWARE_DIR" "$RESOURCES/"

    echo Copying java environment
    for i in antlr.jar jna.jar oro.jar; do
       cp -X "${EDITOR_DIR}/app/lib/$i" "$RESOURCES/"
    done

    echo Copying examples...
    mkdir "$RESOURCES/examples/"
    cp -r "${MIDICTRL_BASE_DIR}/minicommand-sketches/PublishedFirmwares" "$RESOURCES/examples/"
    cp -r "${MIDICTRL_BASE_DIR}/minicommand-sketches/Tutorials" "$RESOURCES/examples/"

    echo Extracting avr tools...
    cp -r "${HARDWARE_DIR}/tools" "$RESOURCES/"
    unzip -q -d "$RESOURCES/" "$TOOLS_ZIP"

fi

### -- START BUILDING -------------------------------------------

# move to root 'processing' directory

### -- BUILD CORE ----------------------------------------------

echo Building processing.core...

cd "${EDITOR_DIR}/core"

#CLASSPATH=/System/Library/Frameworks/JavaVM.framework/Classes/classes.jar:/System/Library/Frameworks/JavaVM.framework/Classes/ui.jar:/System/Library/Java/Extensions/QTJava.zip
#export CLASSPATH

perl preproc.pl

mkdir -p bin
javac -source 1.5 -target 1.5 -d bin \
    src/processing/core/*.java \
    src/processing/xml/*.java

rm -f "$RESOURCES/core.jar"

cd bin && \
    zip -rq "$RESOURCES/core.jar" \
    processing/core/*.class \
    processing/xml/*.class

# head back to "processing/app"
cd "${EDITOR_DIR}/app"

### -- BUILD PDE ------------------------------------------------

echo Building the PDE...

# For some reason, javac really wants this folder to exist beforehand.
rm -rf "${WORK_DIR}/classes"
mkdir "${WORK_DIR}/classes"

# Intentionally keeping this separate from the 'bin' folder
# used by eclipse so that they don't cause conflicts.
javac \
    -Xlint:deprecation \
    -source 1.5 -target 1.5 \
    -classpath "$RESOURCES/core.jar:$RESOURCES/antlr.jar:$RESOURCES/ecj.jar:$RESOURCES/jna.jar:$RESOURCES/oro.jar" \
    -d "${WORK_DIR}/classes" \
    src/processing/app/*.java \
    src/processing/app/debug/*.java \
    src/processing/app/macosx/*.java \
    src/processing/app/preproc/*.java \
    src/processing/app/syntax/*.java \
    src/processing/app/tools/*.java \
    src/processing/app/library/*.java >/dev/null 2>&1 

cd "${WORK_DIR}/classes"
rm -f "$RESOURCES/pde.jar"
zip -0rq "$RESOURCES/pde.jar" .
cd "${BUILD_DIR}"

echo Done.
