load(qttest_p4)
contains(QT_CONFIG,declarative): QT += network declarative
macx:CONFIG -= app_bundle

HEADERS += ../debugutil_p.h
SOURCES += tst_qmldebugservice.cpp \
           ../debugutil.cpp
