load(qttest_p4)

SOURCES += tst_qxml.cpp
QT = core xml

wince*|symbian: {
   addFiles.sources = 0x010D.xml
   addFiles.path = .
   DEPLOYMENT += addFiles
}


