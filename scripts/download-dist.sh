#!/bin/sh

#
# MidiCtrl - Script to download the dist package
#
# (c)  July 2011 - Manuel Odendahl - wesen@ruinwesen.com
#

if [ ! -d mididuino-dist ]; then
    wget http://ruinwesen.com/support-files/mididuino-dist.tar.gz && \
    tar zxvf mididuino-dist.tar.gz && \
    rm mididuino-dist.tar.gz
fi