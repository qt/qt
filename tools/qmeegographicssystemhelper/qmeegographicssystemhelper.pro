TEMPLATE = lib
TARGET = QtMeeGoGraphicsSystemHelper

include(../../src/qbase.pri)

QT += gui
INCLUDEPATH += '../../src/plugins/graphicssystems/meego'

HEADERS = qmeegographicssystemhelper.h qmeegooverlaywidget.h qmeegolivepixmap.h qmeegoruntime.h qmeegolivepixmap_p.h
SOURCES = qmeegographicssystemhelper.cpp qmeegooverlaywidget.cpp qmeegoruntime.cpp qmeegolivepixmap.cpp qmeegographicssystemhelper.h qmeegooverlaywidget.h qmeegolivepixmap.h qmeegoruntime.h qmeegolivepixmap_p.h
