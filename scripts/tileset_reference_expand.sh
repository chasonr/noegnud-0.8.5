rm -f "$1"_*.txt
IFS='
'
i=0
for l in `cat "$1".txt`; do
    echo "$i"'='`~/lower "$l"|cut -f2 -d'='`>>"$1"_number.txt;
    echo `~/lower "$l"|cut -f2 -d'='`'='"$i">>"$1"_alpha.txt.tmp;
    ((i=i+1)); 
done
cat "$1"_alpha.txt.tmp|sort>"$1"_alpha.txt && rm -f "$1"_alpha.txt.tmp
