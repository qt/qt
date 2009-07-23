#!/bin/bash

# This script updates the suite from W3C's server.
#
# NOTE: the files checked out CANNOT be added to Trolltech's
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
