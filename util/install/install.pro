TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += archive \
           package \
	   keygen

win32:SUBDIRS += win
mac:SUBDIRS += mac
