#!/bin/bash

CHK_DIR_EXISTS="test -d"
MKDIR="/bin/mkdir"
SRC_DIR=../../src
OBJ_DIRS="${SRC_DIR}/core/.obj ${SRC_DIR}/modules/QuickLogger/.obj ${SRC_DIR}/modules/MessageManager/.obj"

for DIR in $OBJ_DIRS
	do
		$CHK_DIR_EXISTS $DIR || $MKDIR $DIR
	done
