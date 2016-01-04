#rm -rf ~/sandbox
mkdir ~/sandbox
make -e install PREFIX=$HOME/sandbox CHOWN=true CHGRP=true
