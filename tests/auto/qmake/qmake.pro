load(qttest_p4)
HEADERS += testcompiler.h
SOURCES += tst_qmake.cpp testcompiler.cpp

contains(QT_CONFIG, qt3support): QT += qt3support

cross_compile: DEFINES += QMAKE_CROSS_COMPILED


