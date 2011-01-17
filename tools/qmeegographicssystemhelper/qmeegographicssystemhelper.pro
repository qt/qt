TEMPLATE = lib
TARGET = QtMeeGoGraphicsSystemHelper

include(../../src/qbase.pri)

QT += gui opengl
INCLUDEPATH += '../../src/plugins/graphicssystems/meego'

HEADERS = qmeegographicssystemhelper.h qmeegooverlaywidget.h qmeegolivepixmap.h qmeegoruntime.h qmeegolivepixmap_p.h qmeegofencesync.h qmeegofencesync_p.h qmeegoswitchevent.h
SOURCES = qmeegographicssystemhelper.cpp qmeegooverlaywidget.cpp qmeegoruntime.cpp qmeegolivepixmap.cpp qmeegographicssystemhelper.h qmeegooverlaywidget.h qmeegolivepixmap.h qmeegoruntime.h qmeegolivepixmap_p.h qmeegofencesync.h qmeegofencesync_p.h qmeegofencesync.cpp qmeegoswitchevent.cpp qmeegoswitchevent.h
