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

for lib in QtCore QtGui QtNetwork QtXml QtOpenGL QtSql Qt3Support QtSvg QtScript QtScriptTools QtXmlPatterns QtWebKit QtDBus phonon; do
    if [ ! -d "$BINDIR/lib/${lib}.framework" ]; then
	echo "No framework for $lib!"
        continue
    fi
    cp -R "$BINDIR/lib/${lib}.framework" "$FRAMEWORK_DIR/" >/dev/null 2>&1
    ./fix_prl_paths.pl "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl" "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl.fixed" 
    mv "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl.fixed" "$FRAMEWORK_DIR/${lib}.framework/${lib}.prl" 
    ./fix_config_paths.pl "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/$lib" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}.fixed" 
    mv "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/${lib}.fixed" "$FRAMEWORK_DIR/${lib}.framework/Versions/${VERSION_MAJOR}/$lib" 

    # Make a sym-link to make things compatible with what we had in version before 4.2
    OLDDIR="$PWD"
    cd "$FRAMEWORK_DIR/${lib}.framework/Versions"
    ln -s "${VERSION_MAJOR}" "4.0"
    cd "$OLDDIR"
    # Remove the debug libraries and headers (they are part of another package)
    find "$FRAMEWORK_DIR/${lib}.framework/" -name '*_debug*' -exec rm -f {} \; >/dev/null 2>&1
    find "$FRAMEWORK_DIR/${lib}.framework/" -name Headers -exec rm -rf {} \; >/dev/null 2>&1
done

exit 0
