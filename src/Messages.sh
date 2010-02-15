#! /usr/bin/env bash
PROJECT=kMagnet
extractrc `find . -name \*.rc` > rc.cpp
xgettext --from-code=UTF-8 -C -kde -ci18n -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 \
-ki18ncp:1c,2,3 -ktr2i18n:1 -kI18N_NOOP:1 -kI18N_NOOP2:1c,2 -kaliasLocale \
-kki18n:1 -kki18nc:1c,2 -kki18np:1,2 \
-kki18ncp:1c,2,3 *.cpp -o ../po/$PROJECT.pot
#rm -f *.cpp

cd ../po/
echo "Merging translations..."
catalogs=`find . -name '*.po'`
for cat in $catalogs; do
echo $cat
msgmerge -o $cat.new $cat ${PROJECT}.pot
mv $cat.new $cat
done
echo "Done merging translations"
