TEMPLATE = subdirs
SUBDIRS += v8.pro
!cross_compile: SUBDIRS += mksnapshot snapshot.pro
SUBDIRS += scriptlib.pro
CONFIG += ordered
