#!/bin/bash

GITBRANCH=`git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/'`
DATE=`date +%F`

mkdir /tmp/$DATE-$GITBRANCH
for i in `find . -d 1 -type d`; do
    cd "$i"
    make -f ../Makefile 2>&1 > /dev/null
    mv "$i.hex" "/tmp/$DATE-$GITBRANCH/$i-$DATE-$GITBRANCH.hex"
	  echo Built $i-$DATE-$GITBRANCH
    cd -
done

