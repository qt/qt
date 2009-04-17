load(qttest_p4)
SOURCES  += tst_qgraphicsscene.cpp
RESOURCES += images.qrc
win32:!wince*: LIBS += -lUser32

!wince*:DEFINES += SRCDIR=\\\"$$PWD\\\"
DEFINES += QT_NO_CAST_TO_ASCII

wince*: {
   DEFINES += SRCDIR=\\\".\\\"
   rootFiles.sources = Ash_European.jpg graphicsScene_selection.data
   rootFiles.path = .
   renderFiles.sources = testData\render\*
   renderFiles.path = testData\render
   DEPLOYMENT += rootFiles renderFiles
}

