#!/bin/sh
#############################################################################
##
## Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
## Contact: http://www.qt-project.org/legal
##
## This file is the build configuration utility of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and Digia.  For licensing terms and
## conditions see http://qt.digia.com/licensing.  For further information
## use the contact form at http://qt.digia.com/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 as published by the Free Software
## Foundation and appearing in the file LICENSE.LGPL included in the
## packaging of this file.  Please review the following information to
## ensure the GNU Lesser General Public License version 2.1 requirements
## will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## In addition, as a special exception, Digia gives you certain additional
## rights.  These rights are described in the Digia Qt LGPL Exception
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
##
## $QT_END_LICENSE$
##
#############################################################################

# This script updates the suite from W3C's CVS server.
#
# NOTE: the files checked out CANNOT be added to Qt's
# repository at the moment, due to legal complications. However,
# when the test suite is publically released, it is possible as
# according to W3C's usual license agreements.

echo "*** This script typically doesn't need to be run. Test Suite is not available. So, this test is only a place holder! ***"
exit 0

# Download the test suite
TMPFILE='tmpfile'
wget http://www.w3.org/Style/XSL/XSL-TestSuite.zip -O $TMPFILE
unzip $TMPFILE
rm $TMPFILE


# This is W3C's internal CVS server, not the public dev.w3.org.
# export CVSROOT="fenglich@cvs.w3.org:path is currently unknown"

# echo "*** Enter 'anonymous' as password. ***"
# cvs login

mv catalog.xml catalogUnresolved.xml
xmllint --noent  --output catalog.xml catalogUnresolved.xml
