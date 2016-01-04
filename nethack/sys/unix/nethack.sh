#!/bin/sh
#	SCCS Id: @(#)nethack.sh	3.4	1990/02/26
#	Edited for noeGNUd by Clive Crous 2003/09/01

HACKDIR=/usr/games/lib/nethackdir
export HACKDIR
HACK=$HACKDIR/nethack
MAXNROFPLAYERS=1

cd $HACKDIR
case $1 in
	-s*)
		exec $HACK "$@"
		;;
	*)
		exec $HACK "$@" $MAXNROFPLAYERS
		;;
esac
