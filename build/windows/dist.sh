#!/bin/sh

REVISION=`head -1 ../../todo.txt | awk '{print $1}'`
HARDWAREDIR=../../hardware
DISTDIR=../../../mididuino-dist
TOOLSZIP=${DISTDIR}/macosx/tools-universal.zip
SHAREDDIST=${DISTDIR}/shared

if [ $1 ]
then
  RELEASE=$1
  echo Creating Mididuino release $RELEASE...
else 
  RELEASE=$REVISION
  echo Creating Mididuino distribution for revision $REVISION...
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

# remove any old boogers
rm -rf mididuino
rm -rf mididuino-*

mkdir mididuino
cp -r ../../app/lib mididuino

cd ../../hardware/tools/mididuino && make -f Makefile.windows
cd "${PREVDIR}"

cp ${DISTDIR}/windows/*.dll work/

cp -r ../../hardware work/

if [ $1 ]
then
  # write the release version number into the output directory
  echo $1 > mididuino/lib/version.txt
fi

cp ../../app/lib/antlr.jar mididuino/lib/
#cp ../../app/lib/ecj.jar mididuino/lib/
cp ../../app/lib/jna.jar mididuino/lib/
cp ../../app/lib/oro.jar mididuino/lib/

#echo Copying examples...
#cp -r ../shared/examples mididuino/

#echo Extracting reference...
#unzip -q -d mididuino/ ../shared/reference.zip

unzip -q -d mididuino/hardware ${DISTDIR}/windows/avr_tools.zip

# add java (jre) files
unzip -q -d mididuino ${DISTDIR}/windows/jre.zip

# get platform-specific goodies from the dist dir
PREVDIR=`pwd`
cd ${DISTDIR}/windows/launcher
./launch4j/launch4jc.exe config.xml
cp mididuino.exe "${PREVDIR}/mididuino/mididuino.exe"
cd "${PREVDIR}"

# grab pde.jar and export from the working dir
cp work/lib/pde.jar mididuino/lib/
cp work/lib/core.jar mididuino/lib/

# convert revisions.txt to windows LFs
# the 2> is because the app is a little chatty
unix2dos mididuino/readme.txt 2> /dev/null
unix2dos mididuino/lib/preferences.txt 2> /dev/null
unix2dos mididuino/lib/keywords.txt 2> /dev/null

# remove boogers
find mididuino -name "*.bak" -print0 | xargs -0 rm -rf
find mididuino -name "*~" -print0 | xargs -0 rm -rf
find mididuino -name ".DS_Store" -print0 | xargs -0 rm -rf
find mididuino -name "._*" -print0 | xargs -0 rm -rf
find mididuino -name "Thumbs.db" -print0 | xargs -0 rm -rf

# chmod +x the crew
find mididuino -name "*.html" -print0 | xargs -0 chmod +x
find mididuino -name "*.dll" -print0 | xargs -0 chmod +x
find mididuino -name "*.exe" -print0 | xargs -0 chmod +x
find mididuino -name "*.html" -print0 | xargs -0 chmod +x

# clean out the cvs entries
find mididuino -name "CVS" -print0 | xargs -0 rm -rf
find mididuino -name ".cvsignore" -print0 | xargs -0 rm -rf
find mididuino -name ".svn" -print0 | xargs -0 rm -rf

# zip it all up for release
echo Packaging standard release...
echo
P5=mididuino-$RELEASE
mv mididuino $P5
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

