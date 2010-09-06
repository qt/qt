TEMPLATE = lib
TARGET = qmeegographicssystemhelper 

include(../../src/qbase.pri)

QT += gui
INCLUDEPATH += '../../src/plugins/graphicssystems/meego'

VERSION = 0.1.9
HEADERS = qmeegographicssystemhelper.h qmeegooverlaywidget.h qmeegolivepixmap.h qmeegoliveimage.h qmeegoruntime.h qmeegoliveimage_p.h qmeegolivepixmap_p.h
SOURCES = qmeegographicssystemhelper.cpp qmeegooverlaywidget.cpp qmeegoruntime.cpp qmeegolivepixmap.cpp qmeegoliveimage.cpp
