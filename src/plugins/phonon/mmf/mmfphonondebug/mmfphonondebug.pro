TEMPLATE = lib
TARGET = phonon_mmf_debug
OBJECTDUMP_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/mmf/mmfphonondebug

CONFIG += staticlib

DEFINES += OBJECTDUMP_LIBRARY

HEADERS +=					\
	$$OBJECTDUMP_DIR/objectdump_global.h 	\
	$$OBJECTDUMP_DIR/objectdump.h		\
	$$OBJECTDUMP_DIR/objecttree.h

SOURCES += 					\
	$$OBJECTDUMP_DIR/objectdump.cpp 	\
	$$OBJECTDUMP_DIR/objecttree.cpp

symbian {
	HEADERS += $$OBJECTDUMP_DIR/objectdump_symbian.h
	SOURCES += $$OBJECTDUMP_DIR/objectdump_symbian.cpp

	LIBS += -lcone
	LIBS += -lws32

	TARGET.CAPABILITY = all -tcb

} else {
	SOURCES += $$OBJECTDUMP_DIR/objectdump_stub.cpp
}

TARGET.UID3=0x2001E62A
