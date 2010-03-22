load(qttest_p4)
requires(contains(QT_CONFIG,opengl))
QT += opengl

HEADERS += tst_qglthreads.h
SOURCES += tst_qglthreads.cpp

x11 {
    LIBS += $$QMAKE_LIBS_X11
}
