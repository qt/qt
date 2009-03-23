TEMPLATE = lib
CONFIG += staticlib
CONFIG += qt x11
CONFIG -= dll
TARGET = arq

SOURCES += qarchive.cpp ../keygen/keyinfo.cpp
HEADERS += qarchive.h
!zlib:unix:LIBS += -lz
