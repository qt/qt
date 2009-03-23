TEMPLATE = subdirs


!wince*: SUBDIRS = test stressTest
wince*: SUBDIRS = test
