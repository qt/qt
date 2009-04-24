TEMPLATE = subdirs
CONFIG  += ordered

contains(QT_BUILD_PARTS, tools): {
	SUBDIRS += ../../../tools/assistant/lib/fulltextsearch \
        	   ../../../tools/assistant/lib
}

SUBDIRS += tst_qhelpcontentmodel.pro

