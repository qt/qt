CONFIG += testcase

TARGET = tst_qpainter
QT += testlib
SOURCES  += tst_qpainter.cpp

contains(QT_CONFIG, qt3support): QT += qt3support

wince*|symbian: {
    addFiles.files = drawEllipse drawLine_rop_bitmap drawPixmap_rop drawPixmap_rop_bitmap task217400.png
    addFiles.path = .
    DEPLOYMENT += addFiles
}

wince* {
    DEFINES += SRCDIR=\\\".\\\"
} else:!symbian {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

macx:CONFIG+=insignificant_test # QTQAINFRA-574
