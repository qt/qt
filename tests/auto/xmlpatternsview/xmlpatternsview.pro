load(qttest_p4)
SOURCES += tst_xmlpatternsview.cpp

include (../xmlpatterns.pri)

TARGET = tst_xmlpatternsview

wince*: {
    viewexe.sources = $$QT_BUILD_TREE/xmlpatternsview.exe
    viewexe.path = .
    DEPLOYMENT += viewexe
}
