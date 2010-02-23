load(qttest_p4)
SOURCES += tst_xmlpatternsdiagnosticsts.cpp \
           ../qxmlquery/TestFundament.cpp

include (../xmlpatterns.pri)

TARGET = tst_xmlpatternsdiagnosticsts

contains(QT_CONFIG, xmlpatterns) {
HEADERS += ../xmlpatternsxqts/tst_suitetest.h
SOURCES += ../xmlpatternsxqts/tst_suitetest.cpp
LIBS += -l$$XMLPATTERNS_SDK
}

QT += xml

INCLUDEPATH += $$(QTSRCDIR)/tests/auto/xmlpatternssdk  \
               $$(QTDIR)/include/QtXmlPatterns/private \
               $$(QTSRCDIR)/tests/auto/xmlpatternsxqts \
               ../xmlpatternsxqts                      \
               ../xmlpatternssdk

wince*|symbian {
   catalog.sources = TestSuite Baseline.xml
   catalog.path = .
   DEPLOYMENT += catalog
}
