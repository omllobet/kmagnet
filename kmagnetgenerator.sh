#!/bin/bash
VERSION=0.03
EXTENSIONS="cpp png txt kmp docbook h ui desktop kcfg rc kcfgc sh COPYING ChangeLog README TODO AUTHORS"
SOURCEDIR=/home/martini/projects/kmagnet
TARGETDIR=/home/martini/projects/kmagnet-$VERSION

error() {
   echo "$@"
   exit 1
}

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

rm -rf ${TARGETDIR}/build
tar cjspf kmagnet-$VERSION.tar.bz2 kmagnet-$VERSION/
exit 0 
