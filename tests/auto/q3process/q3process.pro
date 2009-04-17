TEMPLATE    =	subdirs

SUBDIRS	=	cat \
		echo \
		tst
TARGET = tst_q3process
QT += qt3support

#no install rule for subdir
INSTALLS =


