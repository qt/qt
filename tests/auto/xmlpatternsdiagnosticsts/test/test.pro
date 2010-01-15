load(qttest_p4)
SOURCES += tst_xmlpatternsdiagnosticsts.cpp \
           ../../qxmlquery/TestFundament.cpp

include (../../xmlpatterns.pri)

TARGET = tst_xmlpatternsdiagnosticsts
DESTDIR = ..
win32 {
    CONFIG(debug, debug|release): DESTDIR = ../debug
    else: DESTDIR = ../release
}

contains(QT_CONFIG, xmlpatterns) {
HEADERS += ../../xmlpatternsxqts/test/tst_suitetest.h
SOURCES += ../../xmlpatternsxqts/test/tst_suitetest.cpp
LIBS += -l$$XMLPATTERNS_SDK
}

QT += xml

INCLUDEPATH += $$(QTSRCDIR)/tests/auto/xmlpatternsxqts/lib/ \
               $$(QTDIR)/include/QtXmlPatterns/private      \
               $$(QTSRCDIR)/tests/auto/xmlpatternsxqts/test \
               ../../xmlpatternsxqts/test                   \
               ../../xmlpatternsxqts/lib

wince*|symbian {
   catalog.sources = ../TestSuite ../Baseline.xml
   catalog.path = .
   DEPLOYMENT += catalog
}
