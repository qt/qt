load(qttest_p4)
SOURCES  += tst_qwidget_window.cpp

x11 {
    LIBS += $$QMAKE_LIBS_X11
}

CONFIG+=insignificant_test # QTQAINFRA-574
