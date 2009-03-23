load(qttest_p4)
VPATH += ../shared
VPATH += ../qvectornd
INCLUDEPATH += ../shared
INCLUDEPATH += ../../../../src/gui/math3d
DEFINES += FIXED_POINT_TESTS
HEADERS += math3dincludes.h
SOURCES += tst_qvectornd.cpp math3dincludes.cpp
