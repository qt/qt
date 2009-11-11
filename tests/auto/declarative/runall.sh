#!/bin/sh

Xnest :7 2>/dev/null &
sleep 1
trap "kill $!" EXIT
export DISPLAY=:7

make -k -j1 install 2>&1 |
    while read line
    do
        case "$line" in
        make*Error) echo "$line";;
        make*Stop) echo "$line";;
        make*) ;;
        */qmake*) ;;
        */bin/moc*) ;;
        *targ.debug*) ;;
        g++*) ;;
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

