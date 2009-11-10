#!/bin/sh

Xnest :7 2>/dev/null &
trap "kill $!" EXIT
export DISPLAY=:7

make install 2>&1 |
    while read line
    do
        case "$line" in
        make*) ;;
        cd*) ;;
        PASS*) ;;
        QDEBUG*) ;;
        Makefile*) ;;
        Config*) ;;
        Totals*) ;;
        \**) ;;
        ./*) ;;
        *) echo "$line"
        esac
    done

