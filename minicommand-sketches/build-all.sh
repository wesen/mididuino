#!/bin/sh

export BASE_DIR=$1

rm -f .failure

find . -maxdepth 1 -type d | grep -v '^\.$' | grep -v '\.svn' |
while read i; do
	echo -e "\n\n"
	echo "Making $i"
	PREVDIR=`pwd`
	cd "$i"
	if make -f ../Makefile 2>&1 > /dev/null
	then
	    echo "SUCCESS"
	else
	    echo "FAILURE"
	    touch ../.failure
	fi
	
	make -f ../Makefile clean >/dev/null 2>&1
	cd "$PREVDIR"
done

if test -f .failure
then
    FAILURE=0
else
    FAILURE=1
fi

rm -f .failure

exit $FAILURE
