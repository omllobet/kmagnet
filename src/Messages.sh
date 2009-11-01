#! /usr/bin/env bash
#$EXTRACTRC `find . -name \*.rc` >> rc.cpp
#$XGETTEXT *.cpp -o $podir/kMagnet.pot
extractrc `find . -name \*.rc` >> rc.cpp
xgettext -ki18n -kI18N_NOOP  *.cpp -o ../po/kMagnet.pot
#rm -f *.cpp
