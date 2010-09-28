load(qttest_p4)
contains(QT_CONFIG,declarative): QT += network declarative script
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativedebughelper.cpp
