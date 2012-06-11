#!/bin/sh

REVISION=`head -c 4 ../../todo.txt`

rm -rf work

./make.sh

echo Creating linux distribution for revision $REVISION...

rm -rf work/classes

# remove any old boogers
rm -rf midi-ctrl
rm -rf midi-ctrl-*

mv work midi-ctrl

# remove boogers
find midi-ctrl -name "*~" -exec rm -f {} ';'
find midi-ctrl -name ".DS_Store" -exec rm -f {} ';'
find midi-ctrl -name "._*" -exec rm -f {} ';'
find midi-ctrl -name "Thumbs.db" -exec rm -f {} ';'

# clean out the cvs entries
find midi-ctrl -name ".svn" -exec rm -rf {} ';' 2> /dev/null

# zip it all up for release
echo Creating tarball and finishing...
P5=midi-ctrl-$REVISION
mv midi-ctrl $P5

tar cfz $P5.tgz $P5
# nah, keep the new directory around
#rm -rf $P5

#echo Done.
