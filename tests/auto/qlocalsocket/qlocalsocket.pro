TEMPLATE = subdirs
SUBDIRS = lackey test
!wince*: SUBDIRS += example
