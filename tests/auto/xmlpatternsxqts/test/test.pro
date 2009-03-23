load(qttest_p4)
SOURCES += tst_xmlpatternsxqts.cpp ../../qxmlquery/TestFundament.cpp tst_suitetest.cpp

include(../../xmlpatterns.pri)

contains(QT_CONFIG,xmlpatterns) {
    HEADERS += tst_suitetest.h
    LIBS += -l$$XMLPATTERNS_SDK
}

# syncqt doesn't copy headers in tools/ so let's manually ensure
# it works with shadow builds and source builds.
INCLUDEPATH += $$(QTDIR)/include/QtXmlPatterns/private      \
               $$(QTSRCDIR)/include/QtXmlPatterns/private   \
               $$(QTSRCDIR)/tools/xmlpatterns               \
               $$(QTDIR)/tools/xmlpatterns                  \
               ../lib/

CONFIG += testlib
QT += xml
DESTDIR = ..
win32 {
    CONFIG(debug, debug|release): DESTDIR = ../debug
    else: DESTDIR = ../release
}
TARGET = tst_xmlpatternsxqts

include (../../xmlpatterns.pri)
