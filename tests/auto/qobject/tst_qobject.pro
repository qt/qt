load(qttest_p4)
SOURCES  += tst_qobject.cpp

QT = core network gui
contains(QT_CONFIG, qt3support):DEFINES+=QT_HAS_QT3SUPPORT

wince*: {
   addFiles.sources = signalbug.exe
   addFiles.path = .
   DEPLOYMENT += addFiles
}

symbian: {
    addFiles.sources = signalbug.exe
    addFiles.path = \sys\bin
    DEPLOYMENT += addFiles
}
