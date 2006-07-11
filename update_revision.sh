#!/bin/sh

REV_FILE=./revision.m4
M4_START="m4_define([SVN_REVISION], trunk-r"
M4_END=")"

REV=0

if [ -f /usr/bin/svn ]; then
	REV=`svn info | grep Revision: | cut -d" " -f2`;
elif [ -f .svn/entries ]; then
	REV=`cat ./.svn/entries | grep revision= | cut -d\" -f2 | head -n1`;
fi

echo "m4_define([SVN_REVISION], trunk-r$REV)" > $REV_FILE
