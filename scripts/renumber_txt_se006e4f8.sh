#!/bin/sh
renumber () {
    mv -f "$1".txt "$1"_.txt
    count=0
    rm -f "$1".txt
    for i in `cat "$1"_.txt|grep "^#"`
	do
	    echo $i|tr [:upper:] [:lower:]|sed -r "s/^[ ]*[#][ ]*tile[ ]*[0-9]+[ ]*/# tile $count /" >>"$1".txt
	    ((count=count+1))
	done
}

IFS='
'
renumber monsters
renumber objects
renumber other
