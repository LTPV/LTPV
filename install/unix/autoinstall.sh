#/usr/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
sudo ln -s $DIR/../../bin/ltpv /usr/bin/ltpv
sudo ln -s $DIR/../../src/libLTPV.so /usr/lib/libLTPV.so
sudo ln -s $DIR/../../src/ltpv.h /usr/include/ltpv.h

