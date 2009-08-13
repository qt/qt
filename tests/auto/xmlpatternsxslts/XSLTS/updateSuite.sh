# This script updates the suite from W3C's CVS server.
#
# NOTE: the files checked out CANNOT be added to Qt's
# repository at the moment, due to legal complications. However,
# when the test suite is publically released, it is possible as
# according to W3C's usual license agreements.

echo "*** This script typically doesn't need to be run, and it needs to be updated anyway."
exit 1

# This is W3C's internal CVS server, not the public dev.w3.org.
export CVSROOT="fenglich@cvs.w3.org:path is currently unknown"

echo "*** Enter 'anonymous' as password. ***"
cvs login

mv catalog.xml catalogUnresolved.xml
xmllint --noent  --output catalog.xml catalogUnresolved.xml
