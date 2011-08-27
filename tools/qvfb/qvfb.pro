TEMPLATE        = app
CONFIG          += qt warn_on uic
TARGET          = qvfb
DESTDIR         = ../../bin

!win32:!embedded:!mac:CONFIG += x11

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

FORMS           = config.ui
HEADERS         = qvfb.h \
		  qvfbview.h \
		  qvfbratedlg.h \
		  qanimationwriter.h \
                  gammaview.h \
                  qvfbprotocol.h \
                  qvfbshmem.h \
                  qvfbmmap.h

SOURCES         = qvfb.cpp \
		  qvfbview.cpp \
		  qvfbratedlg.cpp \
                  main.cpp \
		  qanimationwriter.cpp \
                  qvfbprotocol.cpp \
                  qvfbshmem.cpp \
                  qvfbmmap.cpp

include(../shared/deviceskin/deviceskin.pri)

contains(QT_CONFIG, opengl) {
	QT += opengl
}

contains(QT_CONFIG, system-png) {
	LIBS += -lpng
} else {
	INCLUDEPATH     += $$QT_SOURCE_TREE/src/3rdparty/libpng
}
contains(QT_CONFIG, system-zlib) {
	LIBS += -lz
} else {
	INCLUDEPATH     += $$QT_SOURCE_TREE/src/3rdparty/zlib
}

unix:x11 {
    HEADERS     += qvfbx11view.h \
                   x11keyfaker.h \
                   qtopiakeysym.h
    SOURCES     += qvfbx11view.cpp \
                   x11keyfaker.cpp
    LIBS += -lXtst
}

RESOURCES	+= qvfb.qrc
