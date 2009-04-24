TEMPLATE = subdirs
SUBDIRS = lackey test
!wince*:!symbian*: SUBDIRS += example
