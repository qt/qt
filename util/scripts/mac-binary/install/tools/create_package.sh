#!/bin/sh

#options parsing
BINDIR=
SRCDIR=
OUTDIR=
DO_DEBUG=yes
while [ -n "$1" ]; do
   echo "$1" | grep '^--*=*' >/dev/null 2>&1 || break
   OPTION=`echo $1 | sed 's,^--\([^=]*\)=\(.*\),\1,'`
   ARG=`echo $1 | sed 's,^--\([^=]*\)=\(.*\),\2,'`
   shift
   case "$OPTION" in
   debug) DO_DEBUG="$ARG" ;;
   qtsrc) SRCDIR="$ARG" ;;
   qtbin) BINDIR="$ARG" ;;
   outdir) OUTDIR="$ARG" ;;
   *) ;;
   esac
done

copyHeader()
{
   HEADER_FILE=$1
   HEADER_DIR=$2 
   mkdir -p $HEADER_DIR
    if [ `wc -l $HEADER_FILE | awk '{print $1;}'` = "1" ]; then
       LINE=`head -1 $HEADER_FILE`
       if echo "$LINE" | grep '^#include "../../' >/dev/null 2>&1; then
           HEADER_FILE=`dirname $HEADER_FILE`/`echo $LINE | sed "s,^#include \"\([^\"]*\)\"\$,\1,"`
       fi
    fi
    cp "$HEADER_FILE" "$HEADER_DIR"
}

copyHeaderDir()
{
    BASEDIR=$1
    DESTDIR=$2
    mkdir -p "$DESTDIR"
    for header in `find "$BASEDIR" -type f`; do
        case $header in
        *_pch.h|*_p.h|*headers.pri)
            continue
            ;;
        *)
            copyHeader "$header" "$DESTDIR"
            ;;
        esac
    done
}

[ -z "$OUTDIR" ] && exit 1

#copy tools
for a in qmake uic uic3 rcc moc Assistant Assistant_adp pixeltool Linguist Designer qt3to4 rccdump lrelease lupdate lconvert findtr qtusagereporter qdoc qcollectiongenerator qhelpconverter qhelpgenerator xmlpatterns qdbus qdbusviewer qdbusxml2cpp qdbuscpp2xml evalextender macdeployqt; do
    EXE=
    if [ -d "${BINDIR}/bin/${a}.app" ]; then
        mkdir -p "$OUTDIR/Developer/Applications/Qt/"
        cp -R "${BINDIR}/bin/${a}.app" "$OUTDIR/Developer/Applications/Qt"
        EXE="$OUTDIR/Developer/Applications/Qt/${a}.app/Contents/MacOS/$a" #in the bundle
        #place it into the tools dir
        mkdir -p "$OUTDIR/Developer/Tools/Qt/"
        ln -s "/Developer/Applications/Qt/${a}.app" "$OUTDIR/Developer/Tools/Qt/${a}.app"
    elif [ -x "${BINDIR}/bin/${a}" -o -x "${BINDIR}/tools/qdoc3/${a}" ]; then
        EXE="$OUTDIR/usr/bin/$a-${VERSION_MAJOR}.${VERSION_MINOR}"
        mkdir -p `dirname $EXE`
        cp "${BINDIR}/bin/$a" "$EXE"
        ln -s "$a-${VERSION_MAJOR}.${VERSION_MINOR}" "$OUTDIR/usr/bin/$a"
        #place it into the tools dir
        mkdir -p "$OUTDIR/Developer/Tools/Qt/"
        ln -s "/usr/bin/$a-${VERSION_MAJOR}.${VERSION_MINOR}" "$OUTDIR/Developer/Tools/Qt/$a"
    fi
    [ -z "$EXE" ] && continue

    #configs
    if [ "$a" = "qmake" ]; then
        mkdir -p "$OUTDIR/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}/mkspecs"
        cp -PR $SRCDIR/mkspecs/* "$OUTDIR/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}/mkspecs"
        sed "s,qt_no_framework,,g" "$BINDIR/mkspecs/qconfig.pri" >"$OUTDIR/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}/mkspecs/qconfig.pri"
        cat >>"$OUTDIR/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}/mkspecs/qconfig.pri" <<EOF
CONFIG += no_mocdepend
EOF
        (cd "$OUTDIR/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}/mkspecs"; rm -f default; ln -s macx-xcode default)
        ../libraries/fix_config_paths.pl -data "/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}" "$EXE" "/tmp/tmp.exe"
        cp "/tmp/tmp.exe" "$EXE"
        rm -f "/tmp/tmp.exe"
    else
        ../libraries/fix_config_paths.pl "$EXE" "/tmp/tmp.exe"
        cp "/tmp/tmp.exe" "$EXE"
        rm -f /tmp/tmp.exe
    fi
    #perms
    strip "$EXE" >/dev/null 2>&1
    chmod a+x "$EXE" 
done

# Do the tool frameworks (Gah, this is copy-past from the libraries create_package.sh).
FRAMEWORK_DIR="$OUTDIR/Library/Frameworks"
mkdir -p "$FRAMEWORK_DIR"
for lib in QtDesigner QtDesignerComponents QtTest QtAssistant QtHelp; do
    if [ ! -d "$BINDIR/lib/${lib}.framework" ]; then
	echo "No framework for $lib!"
        continue
    fi

    cp -R "$BINDIR/lib/${lib}.framework" "$FRAMEWORK_DIR/" >/dev/null 2>&1
    ../libraries/fix_prl_paths.pl "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl" "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl.fixed" 
    mv "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl.fixed" "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl" 
    ../libraries/fix_config_paths.pl "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/$lib" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}.fixed" 
    mv "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}.fixed" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/$lib" 

    # Remove the debug libraries (they are part of another package)
    find "$FRAMEWORK_DIR/${lib}.framework/" -name '*_debug*' -exec rm -f {} \; >/dev/null 2>&1
done

#copy q3porting.xml. qt3to4 looks for it in QLibraryInfo::DataPath and QLibraryInfo::PrefixPath
cp $BINDIR/tools/porting/src/q3porting.xml $OUTDIR/usr/local/Qt${VERSION_MAJOR}.${VERSION_MINOR}/q3porting.xml

# Finally handle QtUiTools
if [ -e "${BINDIR}/lib/libQtUiTools.a" ]; then
    mkdir -p "$OUTDIR/usr/lib"
    cp "${BINDIR}/lib/libQtUiTools.a" "$OUTDIR/usr/lib/libQtUiTools.a"
    # Copy its headers as well
    copyHeaderDir "${BINDIR}/include/QtUiTools" "$OUTDIR/usr/include/QtUiTools"
fi

if [ -e "${BINDIR}/lib/libQtCLucene.${VERSION_MAJOR}.dylib" ]; then
    mkdir -p "$OUTDIR/usr/lib"
    for lib in libQtCLucene.dylib libQtCLucene.${VERSION_MAJOR}.dylib libQtCLucene.${VERSION_MAJOR}.${VERSION_MINOR}.dylib libQtCLucene.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.dylib; do
    cp -R "${BINDIR}/lib/$lib" "$OUTDIR/usr/lib/"
    done
fi

# finally phrase books for Linguist
PHRASEDEST="$OUTDIR/Developer/Applications/Qt/phrasebooks"
mkdir -p "$PHRASEDEST"
for phrasebook in `find $BINDIR/tools/linguist/phrasebooks/ -name '*.qph'`; do
    cp "$phrasebook" "$PHRASEDEST"
done


