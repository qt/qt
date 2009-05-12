TEMPLATE    =	subdirs

SUBDIRS	=	cat \
		echo \
		tst
TARGET = tst_q3process
QT += qt3support
requires(contains(QT_CONFIG,qt3support))

#no install rule for subdir
INSTALLS =


