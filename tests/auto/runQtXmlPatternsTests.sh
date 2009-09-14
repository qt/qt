#!/bin/sh
#############################################################################
##
## Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
## All rights reserved.
## Contact: Nokia Corporation (qt-info@nokia.com)
##
## This file is the build configuration utility of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial Usage
## Licensees holding valid Qt Commercial licenses may use this file in
## accordance with the Qt Commercial License Agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Nokia.
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
## GNU General Public License Usage
## Alternatively, this file may be used under the terms of the GNU
## General Public License version 3.0 as published by the Free Software
## Foundation and appearing in the file LICENSE.GPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU General Public License version 3.0 requirements will be
## met: http://www.gnu.org/copyleft/gpl.html.
##
## If you have questions regarding the use of this file, please contact
## Nokia at qt-info@nokia.com.
## $QT_END_LICENSE$
##
#############################################################################

# Runs all the tests specific to QtXmlPatterns in one go.

# If you add a test, remember to update ./auto.pro too.
# checkxmlfiles is not part of Patternist, but since it shares test code
# and use Patternist, we include it as well.
#
# Sorted alphabetically, with the difference that xmlpatternsxqts appears
# before xmlpattersview, xmlpatternsdiagnosticsts, and xmlpatternsxslts, since
# they have the former as dependency.
tests="                     \
checkxmlfiles               \
patternistexamplefiletree   \
patternistexamples          \
patternistheaders           \
qabstractmessagehandler     \
qabstracturiresolver        \
qabstractxmlforwarditerator \
qabstractxmlnodemodel       \
qabstractxmlreceiver        \
qapplicationargumentparser  \
qautoptr                    \
qsimplexmlnodemodel         \
qsourcelocation             \
qtokenautomaton             \
qxmlformatter               \
qxmlitem                    \
qxmlname                    \
qxmlnamepool                \
qxmlnodemodelindex          \
qxmlquery                   \
qxmlresultitems             \
qxmlserializer              \
xmlpatterns                 \
xmlpatternsxqts             \
xmlpatternsdiagnosticsts    \
xmlpatternsview             \
xmlpatternsxslts"

os=`uname`

for test in $tests; do
    cd $test
    make distclean
    qmake
    make

    if [ $os = "Darwin" ]; then
        ./tst_"$test".app/Contents/MacOS/tst_"$test"
    else
        ./tst_$test
    fi

    if [ $? -ne 0 ]; then
        exit $?
    else
        cd ..
    fi
done
