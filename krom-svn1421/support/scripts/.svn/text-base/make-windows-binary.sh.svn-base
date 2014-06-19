#! /bin/bash

VERSION=''

if [ ! -z "$1" ]; then
    VERSION="$1"
fi

if [ -z "${VERSION}" ]; then
    if [ ! -f RELEASE_VERSION ]; then
        echo "ERROR: RELEASE_VERSION file missing, cannot create"
        exit 1
    fi
    VERSION="`<RELEASE_VERSION`"
fi

if [ -z "${VERSION}" ]; then
    VERSION="testing"
fi

RELEASEDIR="releases"
NPDEST=/tmp/netpanzer
ZIPNAME="netpanzer-windows-${VERSION}.zip"
EXENAME="build/mingw/release/netpanzer.exe"

echo "Making netPanzer windows version ${VERSION}"

scons cross=mingw \
      sdlconfig=~/program/tools/crossmingw/bin/sdl-config \
      "version=${VERSION}"

i486-mingw32-strip "${EXENAME}"

[ -d "${NPDEST}" ] && rm -rf "${NPDEST}"

mkdir -p "${NPDEST}"
cp "${EXENAME}" "${NPDEST}"

support/scripts/copy_datafiles.sh "${NPDEST}"
support/scripts/copy_docfiles.sh "${NPDEST}"

cp ~/program/tools/crossmingw/dll/SDL/* "${NPDEST}"
cp ~/program/tools/crossmingw/dll/SDL_mixer/* "${NPDEST}"

[ ! -d "${RELEASEDIR}" ] && mkdir "${RELEASEDIR}"

pushd "${NPDEST}/.."
zip -9r "${ZIPNAME}" `basename "${NPDEST}"`
popd

[ -f "${RELEASEDIR}/${ZIPNAME}" ] && rm -f "${RELEASEDIR}/${ZIPNAME}"

mv "${NPDEST}/../${ZIPNAME}" "${RELEASEDIR}"
rm -rf "${NPDEST}"

