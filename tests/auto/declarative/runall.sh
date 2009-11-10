#!/bin/sh

Xnest :7 2>/dev/null &
trap "kill $!" EXIT
export DISPLAY=:7

make install 2>&1 |
    while read line
    do
        case "$line" in
        [a-z]*) ;;
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

