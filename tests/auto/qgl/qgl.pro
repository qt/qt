############################################################
# Project file for autotest for file qgl.h
############################################################

load(qttest_p4)
contains(QT_CONFIG, opengl):QT += opengl

SOURCES += tst_qgl.cpp


