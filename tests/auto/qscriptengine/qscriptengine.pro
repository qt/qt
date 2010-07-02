load(qttest_p4)
QT = core gui script
SOURCES += tst_qscriptengine.cpp 
RESOURCES += qscriptengine.qrc

wince* {
    DEFINES += SRCDIR=\\\"./\\\"
} else:!symbian {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

wince*|symbian: {
   addFiles.sources = script
   addFiles.path = .
   DEPLOYMENT += addFiles
}

symbian: {
   TARGET.UID3 = 0xE0340006
   DEFINES += SYMBIAN_SRCDIR_UID=$$lower($$replace(TARGET.UID3,"0x",""))
}
