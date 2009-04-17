#!/bin/sh

DO_DEBUG=yes
[ "$1" = "-debug" ] && DO_DEBUG=yes

#figure out compiler flags
CFLAGS="-F${QTDIR}/lib"
LFLAGS=
for a in QtCore; do
    LFLAGS="$LFLAGS -framework ${a}"
done
[ "$DO_DEBUG" = "yes" ] && CFLAGS="$CFLAGS -g"
if [ -d "/Developer/Applications/Xcode.app/Contents/PlugIns/GDBMIDebugging.xcplugin/Contents/Headers/" ]; then
    CFLAGS="$CFLAGS -I/Developer/Applications/Xcode.app/Contents/PlugIns/GDBMIDebugging.xcplugin/Contents/Headers/"
elif [ -d "/Developer/Applications/Xcode.app/Contents/PlugIns/GDBMIDebugging.pbplugin/Contents/Headers/" ]; then
    CFLAGS="$CFLAGS -I/Developer/Applications/Xcode.app/Contents/PlugIns/GDBMIDebugging.pbplugin/Contents/Headers/"
fi

#link
/usr/bin/c++ -bundle bundle.cpp $CFLAGS -o QtDataFormatters $LFLAGS
