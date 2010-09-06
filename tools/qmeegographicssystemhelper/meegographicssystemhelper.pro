TEMPLATE = lib
TARGET = meegographicssystemhelper 
QT += gui
INCLUDEPATH += '../'
LIBS += -L../plugin
CONFIG += debug
VERSION = 0.1.9
HEADERS = mgraphicssystemhelper.h moverlaywidget.h mlivepixmap.h mliveimage.h mruntime.h mliveimage_p.h mlivepixmap_p.h
SOURCES = mgraphicssystemhelper.cpp mgraphicssystemhelper.h moverlaywidget.h moverlaywidget.cpp mruntime.cpp mruntime.h mlivepixmap.cpp mlivepixmap.h mliveimage.h mliveimage.cpp mliveimage_p.h mlivepixmap_p.h

target.path = /usr/lib/

headers.files =  mgraphicssystemhelper.h moverlaywidget.h mlivepixmap.h mliveimage.h 
headers.path = /usr/include/meegographicssystemhelper/

pkg.files = meegographicssystemhelper.pc
pkg.path = /usr/lib/pkgconfig/

INSTALLS += target headers pkg
