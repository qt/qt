TEMPLATE = subdirs
SUBDIRS += v8
!cross_compile: SUBDIRS += mksnapshot snapshot
SUBDIRS += api
CONFIG += ordered
