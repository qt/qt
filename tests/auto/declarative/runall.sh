#!/bin/sh
#
#############################################################################
##
## Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is part of the test suite of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## No Commercial Usage
## This file contains pre-release code and may not be distributed.
## You may use this file in accordance with the terms and conditions
## contained in the Technology Preview License Agreement accompanying
## this package.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Nokia gives you certain additional
## rights.  These rights are described in the Nokia Qt LGPL Exception
## version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
##
##
##
##
##
##
##
##
## $QT_END_LICENSE$
##
############################################################################/

if [ "$(uname)" = Linux ]
then
    Xnest :7 2>/dev/null &
    sleep 1
    trap "kill $!" EXIT
    export DISPLAY=:7
    export LANG=en_US
    kwin 2>/dev/null &
    sleep 1
fi

function filter
{
    exe=$1
    skip=0
    while read line
    do
        if [ $skip != 0 ]
        then
            let skip=skip-1
        else
            case "$line" in
            make*Error) echo "$line";;
            make*Stop) echo "$line";;
            /*/bin/make*) ;;
            make*) ;;
            install*) ;;
            QDeclarativeDebugServer:*Waiting*) ;;
            QDeclarativeDebugServer:*Connection*) ;;
            */qmake*) ;;
            */bin/moc*) ;;
            *targ.debug*) ;;
            g++*) ;;
            cd*) ;;
            XFAIL*) skip=1;;
            SKIP*) skip=1;;
            PASS*) ;;
            QDEBUG*) ;;
            Makefile*) ;;
            Config*) ;;
            Totals*) ;;
            \**) ;;
            ./*) ;;
            *tst_*) echo "$line" ;;
            *) echo "$exe: $line"
            esac
        fi
    done
}

make -k -j1 install 2>&1 | filter build
for exe in $(make install | sed -n 's/^install .* "\([^"]*qt4\/tst_[^"]*\)".*/\1/p')
do
    echo $exe
    $exe 2>&1 | filter $exe
done

