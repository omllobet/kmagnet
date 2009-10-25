#!/bin/bash
VERSION=0.04
PROGRAM=kmagnet
EXTENSIONS="cpp png txt kmp docbook h ui desktop kcfg rc kcfgc sh COPYING ChangeLog README TODO AUTHORS"
SOURCEDIR=~/projects/$PROGRAM
TARGETDIR=~/projects/$PROGRAM-$VERSION
DOCBOOK=doc/index.docbook
error() {
   echo "$@"
   exit 1
}
#update files with new version number and date
TODAY=$(date +"%Y-%m-%d")
CANVI0='s/<date>[-0-9]*<\/date>/<date>'${TODAY}'<\/date>/'
sed -i $CANVI0 $SOURCEDIR/$DOCBOOK

CANVI1='s/<releaseinfo>[\.0-9]*<\/releaseinfo>/<releaseinfo>'${VERSION}'<\/releaseinfo>/'
sed -i $CANVI1 $SOURCEDIR/$DOCBOOK

CANVI2='s/^Version:\t[\.0-9]*$/Version:\t'${VERSION}'/'
sed -i $CANVI2 $SOURCEDIR/$PROGRAM.spec

IFS=$''
CANVI3='s/static const char version\[\] = \"[\.0-9]*\"\;/static const char version\[\] = \"'${VERSION}'\"\;/'
sed -i $CANVI3 $SOURCEDIR/src/main.cpp

IFS=$'\t\n '
[ "$EXTENSIONS" ] || error "no extensions specified"
[ "${SOURCEDIR}" ] || error "source directory not specified"
[ -d "${SOURCEDIR}" ] || error "source directory not found"
[ "${TARGETDIR}" ] || error "target directory not specified"
[ ! -d "${TARGETDIR}" ] && mkdir -p ${TARGETDIR}
[ -d "${TARGETDIR}" ] || error "target directory creation failed"
pushd "${SOURCEDIR}" || error "can't change directory to source directory"

IFS=$' '
cmd="find -type f | grep"
for a in ${EXTENSIONS}; do cmd="${cmd} -e \.${a}$"; done

IFS=$'\n'
#echo "${cmd}"
for a in $(eval "${cmd}"); do
	cp --parent -v "$a" "${TARGETDIR}"
done

IFS=$' \t\n'
popd

#git archive --format=tar --remote=git://gitorious.org/kmagnet/kmagnet.git
#git tag $VERSION
#wget http://www.gitorious.org/kmagnet/kmagnet/archive-tarball/master

rm -rf ${TARGETDIR}/build
tar cjspf $PROGRAM-$VERSION.tar.bz2 $PROGRAM-$VERSION/
exit 0 
