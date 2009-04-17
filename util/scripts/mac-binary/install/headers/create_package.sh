#!/bin/sh

#options parsing
BINDIR=
SRCDIR=
OUTDIR=
while [ -n "$1" ]; do
   echo "$1" | grep '^--*=*' >/dev/null 2>&1 || break
   OPTION=`echo $1 | sed 's,^--\([^=]*\)=\(.*\),\1,'`
   ARG=`echo $1 | sed 's,^--\([^=]*\)=\(.*\),\2,'`
   shift
   case "$OPTION" in
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
    if [ ! -d "$BINDIR/lib/${lib}.framework/Headers" ]; then
	echo "No headers for $lib"
        continue
    fi
    (cd "$BINDIR/lib/${lib}.framework/"
    for header in `find . -name Headers`; do 
      mkdir -p "$FRAMEWORK_DIR/${lib}.framework/`dirname $header`"
      if [ -L "$header" ]; then
	  cp -RP "$header" "$FRAMEWORK_DIR/${lib}.framework/`dirname $header`"
      else
	  cp -R "$header" "$FRAMEWORK_DIR/${lib}.framework/`dirname $header`"
      fi
    done)
done

exit 0
