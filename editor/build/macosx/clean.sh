#!/bin/sh

rm -rf work MidiCtrl*.dmg

MIDICTRL_BASE_DIR=`pwd`/../../..
HARDWARE_DIR=${MIDICTRL_BASE_DIR}/hardware
EDITOR_DIR=${MIDICTRL_BASE_DIR}/editor

PREVDIR=`pwd`
cd "${HARDWARE_DIR}/tools/mididuino" && make -f Makefile.OSX clean libclean
cd "${PREVDIR}"
