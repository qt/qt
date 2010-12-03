############################################################
# Project file for autotest for file qsystemtrayicon.h
############################################################

load(qttest_p4)

SOURCES += tst_qsystemtrayicon.cpp
win32:LIBS += user32.lib


