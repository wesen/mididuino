#!/bin/bash

BASE_DIR=/Users/manuel/code/mididuino-core/patchmanager/dist/patchmanager
CLASSPATH=$BASE_DIR/lib:$BASE_DIR/patchmanager.jar

java -cp "$CLASSPATH" com.ruinwesen.patchmanager.cli.CLIUtil "$@"
