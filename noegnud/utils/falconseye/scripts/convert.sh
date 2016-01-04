#!/bin/sh
IFS='
'
rm -rf .convert_1
mkdir .convert_1
echo -n 'Making "red screen" true alpha ['
for i in `ls *.pcx`
    do
	echo -n '.'
	convert -transparent '#580000' "$i" .convert_1/`echo "$i"|rev|cut -f2- -d'.'|rev`.xpm
    done
echo ']'
cd .convert_1
rm -rf ../.convert_2
mkdir ../.convert_2
echo -n 'increasing image depth ['
for i in `ls *.xpm`
    do
	echo -n '.'
	convert -depth 32 "$i" ../.convert_2/"$i"
    done
echo ']'
cd ../.convert_2
echo -n 'image cleanup ['
for i in `ls *.xpm`
    do
	echo -n '.'
	convert -despeckle -antialias "$i" ../`echo "$i"|rev|cut -f2- -d'.'|rev`.png
    done
echo ']'
cd ..
rm -rf .convert*
