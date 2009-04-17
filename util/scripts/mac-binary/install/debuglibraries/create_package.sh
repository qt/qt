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
[ -z "$OUTDIR" ] && exit 1

#setup
FRAMEWORK_DIR="$OUTDIR/Library/Frameworks"
mkdir -p "$FRAMEWORK_DIR"

for lib in QtCore QtGui QtNetwork QtXml QtOpenGL QtSql Qt3Support QtSvg QtAssistant QtDesigner QtDesignerComponents QtTest QtScript QtScriptTools QtHelp QtXmlPatterns QtWebKit QtDBus phonon; do
    if [ ! -d "$BINDIR/lib/${lib}.framework" ]; then
	echo "No framework for $lib!"
        continue
    fi
    cp -R "$BINDIR/lib/${lib}.framework" "$FRAMEWORK_DIR/" >/dev/null 2>&1
    ../libraries/fix_config_paths.pl "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}_debug" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}_debug.fixed" 
    mv "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}_debug.fixed" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}_debug" 
    ../libraries/fix_prl_paths.pl "$FRAMEWORK_DIR/${lib}.framework/${lib}_debug.prl" "$FRAMEWORK_DIR/${lib}.framework/${lib}_debug.prl.fixed" 
    mv "$FRAMEWORK_DIR/${lib}.framework/${lib}_debug.prl.fixed" "$FRAMEWORK_DIR/${lib}.framework/${lib}_debug.prl" 
    # Remove the normal libraries and headers (they are part of another package)
    # some find command here.
    find "$FRAMEWORK_DIR/${lib}.framework" -name ${lib} -o -name ${lib}'.*' -a ! -name ${lib}.framework | xargs rm >/dev/null 2>&1
    find "$FRAMEWORK_DIR/${lib}.framework/" -name Headers -exec rm -rf {} \; >/dev/null 2>&1
    # Since we are using dwarf-2 for debugging, we need to store the dSYM bundles as well.
    /usr/bin/dsymutil -o "$FRAMEWORK_DIR/${lib}.framework/${lib}_debug.dSYM" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}_debug"
done

#Handle the libraries in tools as well
mkdir -p "$OUTDIR/usr/lib"

#first QtAssistantClient, and QtUiTools since they are static
for lib in libQtUiTools_debug.a; do
    [ -e  "$BINDIR/lib/${lib}" ] && cp "$BINDIR/lib/${lib}" "$OUTDIR/usr/lib/${lib}"
done

if [ -e "${BINDIR}/lib/libQtCLucene_debug.${VERSION_MAJOR}.dylib" ]; then
    mkdir -p "$OUTDIR/usr/lib"
    for lib in libQtCLucene_debug.dylib libQtCLucene_debug.${VERSION_MAJOR}.dylib libQtCLucene_debug.${VERSION_MAJOR}.${VERSION_MINOR}.dylib libQtCLucene_debug.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.dylib; do
    cp -R "${BINDIR}/lib/$lib" "$OUTDIR/usr/lib/"
    /usr/bin/dsymutil "$OUTDIR/usr/lib/$lib"
    done
fi


# Now for the plugins
for plugin in `find $BINDIR/plugins/ -name 'lib*.dylib'`; do
    if echo `basename $plugin` | grep "_debug" >/dev/null 2>&1; then
        out_plugin=`echo $plugin | sed "s,^$BINDIR,$OUTDIR/Developer/Applications/Qt,g"`
        pluginDirName= `dirname $out_plugin`
        mkdir -p $pluginDirName
        [ -e "$plugin" ] && ../libraries/fix_config_paths.pl "$plugin" "$out_plugin"
        /usr/bin/dsymutil -o "$pluginDirName/$out_plugin.dSYM" "$out_plugin" 
    fi
done

exit 0
