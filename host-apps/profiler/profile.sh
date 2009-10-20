#!/bin/sh

./profiler | avr-addr2line -C -e $1 -f | ./parse.pl
