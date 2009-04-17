#!/bin/bash -e
[ -r $HOME/.bashrc ] && . $HOME/.bashrc
VERSION=$1

PACKAGE=qt-mac-commercial-src-${VERSION}.tar.gz
OSPACKAGE=qt-mac-opensource-src-${VERSION}.tar.gz
EVALPACKAGE=qt-mac-evalpatches-src-${VERSION}.tar.gz

echo Downloading source packages...
curl -O http://tirion.troll.no/~qt/packages/$VERSION/$PACKAGE
curl -O http://tirion.troll.no/~qt/packages/$VERSION/$OSPACKAGE
curl -O http://tirion.troll.no/~qt/packages/$VERSION/$EVALPACKAGE

echo Building commercial package...
./package/mkpackage -packages $HOME/tmp/mac-binary/install -qtpackage $PWD/$PACKAGE >log 2>&1

echo Building opensource package...
./package/mkpackage -packages $HOME/tmp/mac-binary/install -qtpackage $PWD/$OSPACKAGE >>log 2>&1

echo Building eval package...
./package/mkpackage -packages $HOME/tmp/mac-binary/install -qtpackage $PWD/$PACKAGE -licensetype eval >>log 2>&1

echo -n Uploading results...
scp -B outputs/*.dmg qt@tirion:public_html/packages/$VERSION
