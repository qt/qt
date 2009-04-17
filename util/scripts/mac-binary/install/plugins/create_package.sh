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

#copy plugins
for plugin in `find $BINDIR/plugins/ -name 'lib*.dylib'`; do
    echo `basename $plugin` | grep "_debug" >/dev/null 2>&1 && continue
    out_plugin=`echo $plugin | sed "s,^$BINDIR,$OUTDIR/Developer/Applications/Qt,g"`
    mkdir -p `dirname $out_plugin`
    [ -e "$plugin" ] && ../libraries/fix_config_paths.pl "$plugin" "$out_plugin"
done

exit 0
