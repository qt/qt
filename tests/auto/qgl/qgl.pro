CONFIG += testcase

TARGET = tst_qgl
QT += opengl testlib
SOURCES += tst_qgl.cpp

requires(contains(QT_CONFIG,opengl))
contains(QT_CONFIG,egl):DEFINES += QGL_EGL
win32:!wince*: DEFINES += QT_NO_EGL

RESOURCES  = qgl.qrc

CONFIG+=insignificant_test # QTQAINFRA-428
