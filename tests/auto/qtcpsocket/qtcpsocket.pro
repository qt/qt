TEMPLATE = subdirs


!wince*: SUBDIRS = test stressTest
wince* | symbian* : SUBDIRS = test
