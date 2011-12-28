TEMPLATE = subdirs
SUBDIRS = test

requires(contains(QT_CONFIG,private_tests))

!wince*:SUBDIRS += echo
symbian: TARGET.CAPABILITY = NetworkServices

CONFIG+=insignificant_test # QTQAINFRA-428
