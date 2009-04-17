#!/bin/sh
# 
# Hello!
#
# You don't have to run this script unless you are actually updating the test suite.
# For precaution, we therefore have this exit call.


# CVS is retarded when it comes to reverting changes. Remove files it has moved.
find XML-Test-Suite/ -name ".*.?.*" | xargs rm

cd XML-Test-Suite

export CVSROOT=":pserver:anonymous@dev.w3.org:/sources/public"
cvs -q up -C

p4 edit ...
p4 revert `find -name "Entries"` # They only contain CVS timestamps.
xmllint --valid --noent xmlconf/xmlconf.xml --output xmlconf/finalCatalog.xml
p4 revert -a ...
