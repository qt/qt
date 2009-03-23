load(qttest_p4)
contains(QT_CONFIG, qt3support): QT += qt3support
SOURCES  += tst_qpainter.cpp
wince*: {
    DEFINES += SRCDIR=\\\".\\\"
    addFiles.sources = drawEllipse drawLine_rop_bitmap drawPixmap_rop drawPixmap_rop_bitmap task217400.png
    addFiles.path = .
    DEPLOYMENT += addFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}


