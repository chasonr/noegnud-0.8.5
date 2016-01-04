processdir () {
    for i in `ls -p $1|grep /`; do
	mkdir $i
	cd $i
	processdir $WHR/$i ../$2/$i
	cd ..
    done && \
    for i in `ls -p $1|grep -v /`; do
	ln -s "$2""$i"
    done
}

WIW=`pwd`
WHR=`pwd`/$1
rm -rf win/noegnud && \
mkdir win/noegnud && \
cd win/noegnud && \
processdir $WHR ../../../../noegnud/