#!/bin/sh

if [ -n "$1" ]
then
		export BASE_DIR=$1
fi

rm -f .failure .failed .succeeded

find . -maxdepth 1 -type d | grep -v '^\.$' | grep -v '\.svn' | 
while read i; do
	PREVDIR=`pwd`
	if [ -f "$i/$i.pde" ]
	then 
			echo
			echo "Making $i"
			cd "$i"
			if make -f ../Makefile > /dev/null
			then
					echo "SUCCESS"
					echo "$i" >> ../.succeeded
			else
					echo "FAILURE"
					touch ../.failure	
					echo "$i" >> ../.failed
		  fi
	
			make -f ../Makefile clean >/dev/null 2>&1
			cd "$PREVDIR"
	fi
done

echo "succeeded firmwares: "
cat .succeeded
rm -f .succeeded
echo "failed firmwres: "
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
