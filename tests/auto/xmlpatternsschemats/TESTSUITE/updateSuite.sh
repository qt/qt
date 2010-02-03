#!/bin/bash
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
#############################################################################

# This script updates the suite from W3C's server.
#
# NOTE: the files checked out CANNOT be added to Qt's
# repository at the moment, due to legal complications.

DIRECTORY_NAME="xmlschema2006-11-06"
ARCHIVE_NAME="xsts-2007-06-20.tar.gz"

rm -Rf $DIRECTORY_NAME

wget http://www.w3.org/XML/2004/xml-schema-test-suite/xmlschema2006-11-06/$ARCHIVE_NAME
tar -xzf $ARCHIVE_NAME
rm $ARCHIVE_NAME

CVSROOT=:pserver:anonymous@dev.w3.org:/sources/public cvs login
CVSROOT=:pserver:anonymous@dev.w3.org:/sources/public cvs checkout -d xmlschema2006-11-06-new XML/xml-schema-test-suite/2004-01-14/xmlschema2006-11-06 

java net.sf.saxon.Transform -xsl:unifyCatalog.xsl $DIRECTORY_NAME/suite.xml > testSuites.xml
