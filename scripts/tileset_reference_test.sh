rm -rf noegnud_tileset_test
mkdir noegnud_tileset_test
cd noegnud_tileset_test
IFS='
'
for i in `cat ../tileset_references/"$1".txt`; do
    mkdir "$i"
done
cd ..
rm -rf noegnud_tileset_test
