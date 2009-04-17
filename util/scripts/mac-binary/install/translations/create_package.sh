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

#copy translation
TRANSLATIONDEST="$OUTDIR/Developer/Applications/Qt/translations"
mkdir -p "$TRANSLATIONDEST"
for translation in `find $BINDIR/translations/ -name '*.qm'`; do
    cp "$translation" "$TRANSLATIONDEST"
done

# Copy the untranslated, so there's a starting point for them as well.
for translation in `find $BINDIR/translations/ -name '*_untranslated.ts'`; do
    cp "$translation" "$TRANSLATIONDEST"
done

exit 0
