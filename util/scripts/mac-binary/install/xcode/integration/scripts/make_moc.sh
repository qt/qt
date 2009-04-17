#!/bin/sh 

CFG_PREPROCESS_FLAGS=
CFG_MOC="/usr/bin/moc"
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
   -output|-input|-moc) #second arg style
     OPTION=`echo $1 | sed 's,-\([^=]*\),\1,'`
     shift
     ARG=$1
     ;;          
   esac
   shift
   case "$OPTION" in
   moc) CFG_MOC="$ARG" ;;
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

cd "${SOURCE_ROOT}"
for a in $OTHER_CPLUSPLUSFLAGS; do
   case "$a" in
     -D*|-I*)
       CFG_PREPROCESS_FLAGS="$CFG_PREPROCESS_FLAGS ${a}" ;;
     *) ;;
   esac
done
for a in $GCC_PREPROCESSOR_DEFINITIONS; do
   CFG_PREPROCESS_FLAGS="$CFG_PREPROCESS_FLAGS -D${a}"
done
CFG_PREPROCESS_FLAGS="$CFG_PREPROCESS_FLAGS -I${SOURCE_ROOT}" 

NO_CHANGE=yes
MOC_OUTPUT="${TEMP_FILE_DIR}/moc.out"
echo >"$MOC_OUTPUT"

#do the moc
if [ -e "$CFG_INPUT" ]; then
   if [ '!' -e "$CFG_OUTPUT" ] || [ "$CFG_INPUT" -nt "$CFG_OUTPUT" ]; then
       NO_CHANGE=no
   else
      LINE=0
      LINES=`wc -l "$CFG_INPUT" | awk '{ print $1; }'`
      while [ "$LINE" -lt "$LINES" ]; do
         LINE=$((LINE+1))
         SOURCE=`head -$LINE ${CFG_INPUT} | tail -1`

         FILE=`echo $SOURCE | sed "s,^#include \"\([^\"]*\)\"$,\1,"`
         if [ '!' -f "$FILE" ] || [ "$FILE" -nt "$CFG_OUTPUT" ]; then
             NO_CHANGE=no
             break
         fi
      done
   fi

   if [ "$NO_CHANGE" = "no" ]; then
      LINE=0
      LINES=`wc -l "$CFG_INPUT" | awk '{ print $1; }'`
      while [ "$LINE" -lt "$LINES" ]; do
         LINE=$((LINE+1))
         SOURCE=`head -$LINE ${CFG_INPUT} | tail -1`

         FILE=`echo $SOURCE | sed "s,^#include \"\([^\"]*\)\"$,\1,"`
         if [ -f "$FILE" ]; then
             $CFG_MOC $CFG_PREPROCESS_FLAGS "$FILE" >>"$MOC_OUTPUT"
         fi
      done
   fi
fi

#replace it
if [ "$NO_CHANGE" = "yes" ] || cmp -s "$MOC_OUTPUT" "$CFG_OUTPUT"; then
   rm -f "$MOC_OUTPUT"
else
   mv "$MOC_OUTPUT" "$CFG_OUTPUT"
fi

