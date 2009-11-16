#!/bin/sh

PATCHCLI="../../patchmanager/patchcli.sh"
FIRMWARENAME=$1
DATE=`date +%F--%H-%M`

PASSWORD=$2
TAGS=`cat $FIRMWARENAME/data/tags.txt`
COMMENT=`cat $FIRMWARENAME/data/comment.txt`

function getExistingFirmwareId() {
		"$PATCHCLI" -username wesen -password "$PASSWORD" list | sort -n | grep "$1" | grep -v deleted | cut -f1 -d\ 
}

FIRMID=`getExistingFirmwareId $FIRMWARENAME`
echo "ID $FIRMID"

#echo "Enter password:"
#read PASSWORD

git tag "$FIRMWARENAME-$DATE"
if [ $FIRMID ]; then
		"$PATCHCLI" -username wesen -password "$PASSWORD" delete "$FIRMID"
fi

"$PATCHCLI" -username wesen -password "$PASSWORD" -device minicommand-1.1 \
		-title "$FIRMWARENAME" -environment mididuino-014 -tags "$TAGS" -comment "$COMMENT" -doc "$FIRMWARENAME/data/doc.txt" \
		upload "$FIRMWARENAME/$FIRMWARENAME.hex"

FIRMID=`getExistingFirmwareId $FIRMWARENAME`

"$PATCHCLI" -username wesen -password "$PASSWORD" approve "$FIRMID"