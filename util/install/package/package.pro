TEMPLATE = app
SOURCES = main.cpp
INCLUDEPATH += ../archive $$QT_SOURCE_TREE/include ../keygen
CONFIG += qt console
TARGET  = package
win32:DESTDIR = ../../../bin
unix:LIBS += -L$$QT_BUILD_TREE/util/install/archive -larq
win32:LIBS += ../archive/arq.lib
DEFINES -= UNICODE

win32:!shared:SOURCES += \
	../../../src/3rdparty/zlib/adler32.c \
	../../../src/3rdparty/zlib/compress.c \
	../../../src/3rdparty/zlib/crc32.c \
	../../../src/3rdparty/zlib/deflate.c \
	../../../src/3rdparty/zlib/gzio.c \
	../../../src/3rdparty/zlib/infblock.c \
	../../../src/3rdparty/zlib/infcodes.c \
	../../../src/3rdparty/zlib/inffast.c \
	../../../src/3rdparty/zlib/inflate.c \
	../../../src/3rdparty/zlib/inftrees.c \
	../../../src/3rdparty/zlib/infutil.c \
	../../../src/3rdparty/zlib/trees.c \
	../../../src/3rdparty/zlib/uncompr.c \
	../../../src/3rdparty/zlib/zutil.c
