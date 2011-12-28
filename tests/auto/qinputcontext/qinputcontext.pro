load(qttest_p4)
SOURCES  += tst_qinputcontext.cpp

contains(QT_CONFIG, webkit):QT += webkit

symbian {
    LIBS += -lws32 -lcone
}

CONFIG+=insignificant_test # QTQAINFRA-428
