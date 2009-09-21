#!/bin/sh

#make -f Makefile libclean
make -C MDMonsterNew -f ../Makefile clean all
make -C MDMonsterMelody -f ../Makefile clean all
make -C MDMonsterRandomize -f ../Makefile clean all

ZIPDIR=/tmp/monster-firmwares

rm -rf $ZIPDIR
mkdir $ZIPDIR
cp MDMonsterNew/MDMonsterNew.hex $ZIPDIR
cp MDMonsterMelody/MDMonsterMelody.hex $ZIPDIR
cp MDMonsterRandomize/MDMonsterRandomize.hex $ZIPDIR
cd /tmp && zip -r monster-firmwares.zip monster-firmwares && cd -
mv /tmp/monster-firmwares.zip monster-firmwares-`date +%F`.zip
