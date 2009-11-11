load(qttest_p4)
contains(QT_CONFIG,declarative): QT += network declarative
macx:CONFIG -= app_bundle

HEADERS += ../debuggerutil_p.h
SOURCES += tst_qmldebug.cpp \
           ../debuggerutil.cpp
