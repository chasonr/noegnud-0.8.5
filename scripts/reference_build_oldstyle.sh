convert_map() {
for i in `cat "$1".map`
    do
	current_tileno=`echo $i|cut -f2 -d' '`
	current_description=`echo $i|cut -f2 -d'"'|tr [:upper:] [:lower:]|sed -r 's/[\][,]//g'|sed -r 's/[\][/]/./g'|sed -r 's/, /./g'|sed -r 's/[\]//g'|sed -r 's/[/].*//g'|sed -r "s/['']//g"|sed -r 's/[.][*][.]/./g'|sed -r 's/[ ][.][ ]/./g'|sed -r 's/ +$//'`
	txt_match=`grep '# tile '"$current_tileno"' .*/' "$1.txt"|cut -f2 -d'/'|cut -f1 -d')'|tr [:upper:] [:lower:]|sed -r 's/^ +//'|sed -r 's/ +$//'|sed -r 's/, /./g'|sed -r 's/swallow[ ]/swallow./'`
	fullline=`echo "$current_description"|sed -r "s/[.]$txt_match\$//"|sed -r 's/[. ]$//'``echo '.'"$txt_match"|sed -r 's/[.]$//'`
        echo $counter=$fullline
	#echo '['$current_tileno'  --  '$current_description'  ('$last_item') "'$txt_match'" ] : '$i
	((counter=counter+1))
    done
}


IFS='
'
counter=0
convert_map "monsters"
convert_map "objects"
convert_map "other"

