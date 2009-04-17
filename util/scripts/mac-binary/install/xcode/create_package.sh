#!/bin/sh

#options parsing
BINDIR=
OUTDIR=
while [ -n "$1" ]; do
   OPTION=`echo $1 | sed 's,^--\([^=]*\)=\(.*\),\1,'`
   ARG=`echo $1 | sed 's,^--\([^=]*\)=\(.*\),\2,'`
   shift
   case "$OPTION" in
   qtbin) BINDIR="$ARG" ;;
   outdir) OUTDIR="$ARG" ;;
   *) ;;
   esac
done
[ -z "$OUTDIR" ] && exit 1

translate_cp()
{
    DO_TRANSLATE="$PWD/do_translate.sh"
    if [ -d "$1" ]; then
	(cd "$1" && find . -type f -exec "$DO_TRANSLATE" {} "$2" \;)
    else
	"$DO_TRANSLATE" "$1" "$2"
    fi
}

#copy the scripts
SCRPT_DIR="$OUTDIR/Library/Frameworks/QtCore.framework/Versions/${VERSION_MAJOR}.${VERSION_MINOR}/Resources/xcode"
mkdir -p "$SCRPT_DIR"
translate_cp integration/scripts "$SCRPT_DIR"
for a in $SCRPT_DIR/*.sh; do
  chmod a+x "$a"
done
ln -sf "Versions/${VERSION_MAJOR}.${VERSION_MINOR}/Resources" "$OUTDIR/Library/Frameworks/QtCore.framework/Resources"

#copy the templates
mkdir -p "$OUTDIR/Library/Application Support/Apple/Developer Tools/"
translate_cp integration/templates "$OUTDIR/Library/Application Support/Apple/Developer Tools/"

#build the bundle
(cd "$OUTDIR/Library/Application Support/Apple/Developer Tools/CustomDataViews/QtDataFormatters.bundle/Contents/MacOS" 
chmod 755 ./build_bundle.sh
QTDIR="$BINDIR" ./build_bundle.sh)

exit 0
