#!/bin/sh
#############################################################################
##
## Copyright (C) 2015 The Qt Company Ltd.
## Contact: http://www.qt.io/licensing/
##
## This file is the build configuration utility of the Qt Toolkit.
##
## $QT_BEGIN_LICENSE:LGPL$
## Commercial License Usage
## Licensees holding valid commercial Qt licenses may use this file in
## accordance with the commercial license agreement provided with the
## Software or, alternatively, in accordance with the terms contained in
## a written agreement between you and The Qt Company. For licensing terms
## and conditions see http://www.qt.io/terms-conditions. For further
## information use the contact form at http://www.qt.io/contact-us.
##
## GNU Lesser General Public License Usage
## Alternatively, this file may be used under the terms of the GNU Lesser
## General Public License version 2.1 or version 3 as published by the Free
## Software Foundation and appearing in the file LICENSE.LGPLv21 and
## LICENSE.LGPLv3 included in the packaging of this file. Please review the
## following information to ensure the GNU Lesser General Public License
## requirements will be met: https://www.gnu.org/licenses/lgpl.html and
## http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
##
## As a special exception, The Qt Company gives you certain additional
## rights. These rights are described in The Qt Company LGPL Exception
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
## $QT_END_LICENSE$
##
#############################################################################

# This script updates the suite from W3C's CVS server.
#
# NOTE: the files checked out CANNOT be added to Qt's
# repository at the moment, due to legal complications. However,
# when the test suite is publically released, it is possible as
# according to W3C's usual license agreements.

echo "*** This script typically doesn't need to be run."

# There are two ways to retrieve test suites, via  cvs or direct downloading.
# CVS always receive the latest release.

# download test suite from http://dev.w3.org/2006/xquery-test-suite/

TMPFILE='tmpfile'
wget http://dev.w3.org/2006/xquery-test-suite/PublicPagesStagingArea/XQTS_1_0_3.zip -O $TMPFILE
unzip $TMPFILE
rm $TMPFILE

# This is W3C's internal CVS server, not the public dev.w3.org.
# export CVSROOT=":pserver:anonymous@dev.w3.org:/sources/public"

# echo "*** Enter 'anonymous' as password. ***"
# cvs login
# cvs get 2006/xquery-test-suite

# Substitute entity values for entity references
mv XQTSCatalog.xml XQTSCatalogUnsolved.xml
xmllint -noent -output XQTSCatalog.xml XQTSCatalogUnsolved.xml

