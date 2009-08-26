TEMPLATE = subdirs


!wince*: SUBDIRS = test stressTest
wince*|symbian*|vxworks* : SUBDIRS = test
