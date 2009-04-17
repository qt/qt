#!/bin/sh

CFG_UIC="/usr/bin/uic"
CFG_INPUT=
CFG_OUTPUT=

while [ -n "$1" ]; do
   CACHE_1="$1"
   OPTION=
   ARG=
   case $1 in
   -*=*) #gnu style
     OPTION=`echo $1 | sed 's,-\([^=]*\)=\(.*\),\1,'`
     ARG=`echo $1 | sed 's,-\([^=]*\)=\(.*\),\2,'`
     ;;
   -output|-input|-uic) #second arg style
     OPTION=`echo $1 | sed 's,-\([^=]*\),\1,'`
     shift
     ARG=$1
     ;;          
   esac
   shift
   case "$OPTION" in
   uic) CFG_UIC="$ARG" ;;
   output) CFG_OUTPUT="$ARG" ;;
   input)  CFG_INPUT="$ARG" ;;
   help|*)
       [ "$OPTION" = "help" ] || echo "Unknown option $CACHE_1!" 
       echo "Help!!"
       exit 888;
       ;;
   esac  
done

if [ -z "$CFG_INPUT" ] || [ -z "$CFG_OUTPUT" ]; then
   echo "No input/output file specified."
   exit 1
fi
[ "$CFG_OUTPUT" -nt "$CFG_INPUT" ] && exit 0
cd "${SOURCE_ROOT}"

#do the uic
$CFG_UIC -o "$CFG_OUTPUT" "$CFG_INPUT" 
