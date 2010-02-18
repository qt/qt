############################################################
# Project file for autotest for file qgl.h
############################################################

load(qttest_p4)
requires(contains(QT_CONFIG,opengl))
QT += opengl

HEADERS += tst_openglthreading.h
SOURCES += tst_openglthreading.cpp

