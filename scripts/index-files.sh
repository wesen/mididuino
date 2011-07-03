#!/bin/sh

(cscope-indexer -r -x work -i /dev/stdout -l ; cscope-indexer -r -i /dev/stdout -l /usr/local/CrossPack-AVR/ ) > cscope.files
cscope -b

