#!/bin/sh

[ -z "$TEMP_FILE_DIR" ] && TEMP_FILE_DIR="/tmp"
CFG_MODE=overwrite
CFG_PREPROCESS_FLAGS=
CFG_INPUT=
CFG_OUTPUT=
CFG_MOC="/usr/bin/moc"
CFG_FILE_LIST="/Library/Frameworks/QtCore.framework/Resources/xcode/get_file_list.applescript"

mocable()
{
    if grep '\bQ_OBJECT\b' "$1" >/dev/null 2>&1; then
	return 0
    fi
    return 1
}

while [ -n "$1" ]; do
   CACHE_1="$1"
   OPTION=
   ARG=
   case $1 in
   -*=*) #gnu style
     OPTION=`echo $1 | sed 's,-\([^=]*\)=\(.*\),\1,'`
     ARG=`echo $1 | sed 's,-\([^=]*\)=\(.*\),\2,'`
     ;;
   -output|-moc|-file-list|-input) #second arg style
     OPTION=`echo $1 | sed 's,-\([^=]*\),\1,'`
     shift
     ARG=$1
     ;;
   esac
   shift
   case "$OPTION" in
   file-list) CFG_FILE_LIST="$ARG" ;;
   moc) CFG_MOC="$ARG" ;;
   input) CFG_INPUT="$ARG" ;;
   output) CFG_OUTPUT="$ARG" ;;
   help|*)
       [ "$OPTION" = "help" ] || echo "Unknown option $CACHE_1!"
       echo "Help!!"
       exit 888;
       ;;
   esac
done

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

MOC_LIST="${TEMP_FILE_DIR}/mocable.list"
echo >"$MOC_LIST"

if [ ! -z "$CFG_INPUT" ] && [ ! -z "$CFG_OUTPUT" ]; then
       FILE_TYPE=unknown
       case "$CFG_INPUT" in
       *.cpp|*.C|*.cc|*.CPP|*.CC) FILE_TYPE=c++-source ;;
       *.h|*.H|*.hpp|*.HPP) FILE_TYPE=c++-header ;;
       esac
       [ "$FILE_TYPE" = "unknown" ] && exit 0

       if mocable "$CFG_INPUT"; then
           if [ "$FILE_TYPE" = "c++-source" ]; then
                MOC_FILE=`echo $CFG_INPUT | sed "s,\.[^.]*$,.moc,"`
                if [ ! -e "$MOC_FILE" ] || [ "$CFG_INPUT" -nt "$MOC_FILE" ]; then
                    $CFG_MOC "$CFG_PREPROCESS_FLAGS" "$CFG_INPUT" -o "$MOC_FILE"
                fi
           elif [ "$FILE_TYPE" = "c++-header" ]; then
	        [ -f "$CFG_OUTPUT" ] && cp "$CFG_OUTPUT" "$MOC_LIST"
		echo "#include \"$CFG_INPUT\"" >>"$MOC_LIST"
           fi
       elif [ -f "$CFG_OUTPUT" ]; then
           grep -v "$CFG_INPUT" "$CFG_OUTPUT" >"$MOC_LIST"
       fi
else
       FILE_LIST="${TEMP_FILE_DIR}/file.list"
       osascript "$CFG_FILE_LIST" | sed "s,^[, ]*,,g" >"$FILE_LIST"

       LINE=0
       LINES=`wc -l "$FILE_LIST" | awk '{ print $1; }'`
       while [ "$LINE" -lt "$LINES" ]; do
          LINE=$((LINE+1))
          FILE=`head -$LINE ${FILE_LIST} | tail -1`
          FILE_TYPE=unknown
          case "$FILE" in
          *.cpp|*.C|*.cc|*.CPP|*.CC) FILE_TYPE=c++-source ;;
          *.h|*.H|*.hpp|*.HPP) FILE_TYPE=c++-header ;;
          *.mocs) [ -z "$CFG_OUTPUT" ] && CFG_OUTPUT="$FILE" ;;
          esac
          [ "$FILE_TYPE" = "unknown" ] && continue

          if mocable "$FILE"; then
             if [ "$FILE_TYPE" = "c++-source" ]; then
                  MOC_FILE=`echo $FILE | sed "s,\.[^.]*$,.moc,"`
                  if [ ! -e "$MOC_FILE" ] || [ "$FILE" -nt "$MOC_FILE" ]; then
                      $CFG_MOC "$CFG_PREPROCESS_FLAGS" "$FILE" -o "$MOC_FILE"
                  fi
             elif [ "$FILE_TYPE" = "c++-header" ]; then
                  echo "#include \"$FILE\"" >>"$MOC_LIST"
             fi
          fi
       done
       rm -f "$FILE_LIST"

       if [ -z "$CFG_OUTPUT" ]; then
           echo "No output file specified!"
           exit 1
      fi
#      CFG_MODE=remove
fi

#sort it
sort "$MOC_LIST" | uniq | grep -v '^$' >"${MOC_LIST}.sorted" 
mv "${MOC_LIST}.sorted" "${MOC_LIST}"

#replace it
if cmp -s "$MOC_LIST" "$CFG_OUTPUT"; then
   rm -f "$MOC_LIST"
else
   if [ "$CFG_MODE" = "overwrite" ]; then
       mv "$MOC_LIST" "$CFG_OUTPUT"
   else
       if [ -z `cat "$MOC_LIST"` ]; then
           if [ ! -e "$CFG_OUTPUT" ] || [ ! -z `cat "$CFG_OUTPUT"` ]; then
               echo >"$CFG_OUTPUT"
           fi
       else
	   rm -f "$CFG_OUTPUT"
       fi
       rm -f "$MOC_LIST"
   fi
fi

exit 0
