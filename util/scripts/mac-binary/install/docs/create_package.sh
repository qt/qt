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

mkdir -p "$OUTDIR/Developer/Documentation/Qt"
for dir in `find "$SRCDIR/doc" -name 'html*' -type d`; do
    cp -R "$dir" "$OUTDIR/Developer/Documentation/Qt"
done

for dir in `find "$SRCDIR/doc" -name 'qch' -type d`; do
    cp -R "$dir" "$OUTDIR/Developer/Documentation/Qt"
done

# The old code.
#cp -R "$SRCDIR/doc/html/" "$OUTDIR/Developer/Documentation/Qt"
#[ "$SRCDIR" != "$BINDIR" ] && cp -R "$BINDIR/doc/html/" "$OUTDIR/Developer/Documentation/Qt"
#ln -s . "$OUTDIR/Developer/Documentation/Qt/html"

exit 0
