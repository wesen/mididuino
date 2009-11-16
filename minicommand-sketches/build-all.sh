#!/bin/sh

if [ -n "$1" ]
then
		export BASE_DIR=$1
fi

ORIG_DIR=`pwd`

rm -f .failure .failed .succeeded

touch .failed .succeeded

FIRMWARE_DIRS=PublishedFirmwares

for dir in $FIRMWARE_DIRS; do
		find $dir -maxdepth 1 -type d | grep -v '^\.$' | grep -v '\.svn' |
		while read i; do
				FIRMWARE_NAME=`echo "$i" | sed -e s/.*\\\/\//`
				echo
				echo "$FIRMWARE_NAME"
				PREVDIR=`pwd`
				if [ -f "$i/$FIRMWARE_NAME.pde" ]
				then 
						echo "Making $i"
						cd "$i"
						if make -f "$ORIG_DIR/Makefile" clean all > /dev/null
						then
								echo "SUCCESS"
								avr-size "$FIRMWARE_NAME.elf"
								echo "$i" >> "$ORIG_DIR/.succeeded"
						else
								echo "FAILURE"
								touch "$ORIG_DIR/.failure"
								echo "$i" >> "$ORIG_DIR/.failed"
						fi
	
#						make -f "$ORIG_DIR/Makefile" clean >/dev/null 2>&1
						cd "$PREVDIR"
				fi
		done
done

echo
echo "SUCCESS FIRMWARES: "
cat .succeeded
rm -f .succeeded

echo
echo "FAILED FIRMWARES: "
cat .failed
rm -f .failed

if test -f .failure
then
    FAILURE=0
else
    FAILURE=1
fi

rm -f .failure

exit $FAILURE
