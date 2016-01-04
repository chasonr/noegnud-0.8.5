for i in `ls variants`; do
    echo "Creating links for : $i"
    cd variants/$i
    sh ../../scripts/variant_link_create_single.sh ../../noegnud/
    cd ../../
done
