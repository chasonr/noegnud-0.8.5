#!/bin/sh
rm -rf tmp
mkdir tmp
cat monsters.txt objects.txt other.txt |grep '#'|cut -d'(' -f2|cut -d')' -f1|tr [:upper:] [:lower:]>tmp/all.txt
mkdir -p tmp/txt
c=0
IFS='
'
for i in `cat tmp/all.txt`
  do
    echo "$i"|grep '/'|cut -f2 -d'/'|sed s/^\ //>tmp/txt/"$c"
    ((c=c+1))
  done
cat monsters.map objects.map other.map|cut -f2 -d'"'|tr [:upper:] [:lower:]|sed s/\,\ /\./|sed s/\,\ /\./|sed s/\,\ /\./|sed s/\,\ /\./|sed s/\,\ /\./|sed s/\*\.//|sed s/\\\\\\//\./|sed s/\\\\//|sed s/\ \\/\ /\./|sed s/\'//>tmp/all.map
c=0
for i in `cat tmp/all.map`
  do
    echo "$c"="$i"'.'`cat tmp/txt/$c`|sed s/\\.$//
    ((c=c+1))
  done
rm -rf tmp
