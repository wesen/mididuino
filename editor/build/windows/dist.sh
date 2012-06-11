#!/bin/sh

REVISION=`head -1 ../../todo.txt | awk '{print $1}'`
HARDWAREDIR=../../hardware
DISTDIR=../../../mididuino-dist
TOOLSZIP=${DISTDIR}/macosx/tools-universal.zip
SHAREDDIST=${DISTDIR}/shared

if [ $1 ]
then
  RELEASE=$1
  echo Creating Midi-Ctrl release $RELEASE...
else 
  RELEASE=$REVISION
  echo Creating Midi-Ctrl distribution for revision $REVISION...
fi

# check to see if the version number in the app is correct
# so that mikkel doesn't kick my ass
VERSIONED=`cat ../../app/src/processing/app/Base.java | grep $REVISION`
if [ -z "$VERSIONED" ]
then
  echo Fix the revision number in Base.java
  exit
fi

./make.sh

echo `pwd`

# remove any old boogers
rm -rf midi-ctrl
rm -rf midi-ctrl-*

mkdir midi-ctrl
cp -r ../../app/lib midi-ctrl

PREVDIR=`pwd`
cd ../../hardware/tools/mididuino && make -f Makefile.windows
cd "${PREVDIR}"

cp ${DISTDIR}/windows/*.dll midi-ctrl/

cp -r ../../hardware midi-ctrl/
cp ../../readme.txt midi-ctrl
cp ../../license.txt midi-ctrl
cp ../../building.txt midi-ctrl

if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > midi-ctrl/lib/version.txt
fi

cp ../../app/lib/antlr.jar midi-ctrl/lib/
#cp ../../app/lib/ecj.jar midi-ctrl/lib/
cp ../../app/lib/jna.jar midi-ctrl/lib/
cp ../../app/lib/oro.jar midi-ctrl/lib/

#echo Copying examples...
#cp -r ../shared/examples midi-ctrl/

#echo Extracting reference...
#unzip -q -d midi-ctrl/ ../shared/reference.zip

unzip -q -d midi-ctrl/hardware ${DISTDIR}/windows/avr_tools.zip

# add java (jre) files
unzip -q -d midi-ctrl ${DISTDIR}/windows/jre.zip

# get platform-specific goodies from the dist dir
PREVDIR=`pwd`
cd ${DISTDIR}/windows/launcher
./launch4j/launch4jc.exe config.xml
cp mididuino.exe "${PREVDIR}/midi-ctrl/midi-ctrl.exe"
cd "${PREVDIR}"

# grab pde.jar and export from the working dir
cp work/lib/pde.jar midi-ctrl/lib/
cp work/lib/core.jar midi-ctrl/lib/

# convert revisions.txt to windows LFs
# the 2> is because the app is a little chatty
unix2dos midi-ctrl/readme.txt 2> /dev/null
unix2dos midi-ctrl/lib/preferences.txt 2> /dev/null
unix2dos midi-ctrl/lib/keywords.txt 2> /dev/null

# remove boogers
find midi-ctrl -name "*.bak" -print0 | xargs -0 rm -rf
find midi-ctrl -name "*~" -print0 | xargs -0 rm -rf
find midi-ctrl -name ".DS_Store" -print0 | xargs -0 rm -rf
find midi-ctrl -name "._*" -print0 | xargs -0 rm -rf
find midi-ctrl -name "Thumbs.db" -print0 | xargs -0 rm -rf

# chmod +x the crew
find midi-ctrl -name "*.html" -print0 | xargs -0 chmod +x
find midi-ctrl -name "*.dll" -print0 | xargs -0 chmod +x
find midi-ctrl -name "*.exe" -print0 | xargs -0 chmod +x
find midi-ctrl -name "*.html" -print0 | xargs -0 chmod +x

# clean out the cvs entries
find midi-ctrl -name "CVS" -print0 | xargs -0 rm -rf
find midi-ctrl -name ".cvsignore" -print0 | xargs -0 rm -rf
find midi-ctrl -name ".svn" -print0 | xargs -0 rm -rf

# zip it all up for release
echo Packaging standard release...
echo
P5=midi-ctrl-$RELEASE
mv midi-ctrl $P5
zip -rq $P5.zip $P5
# nah, keep the new directory around
#rm -rf $P5

# zip up another for experts
#echo Expert release is disabled until further notice.
echo Packaging expert release...
echo
cp -a $P5 $P5-expert
# remove enormous java runtime
rm -rf $P5-expert/java
zip -rq $P5-expert.zip $P5-expert

echo Done.

