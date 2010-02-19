load(qttest_p4)
SOURCES += tst_xmlpatternsxslts.cpp \
           ../qxmlquery/TestFundament.cpp

include (../xmlpatterns.pri)

contains(QT_CONFIG,xmlpatterns) {
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

wince*: {
    testdata.sources = XSLTS Baseline.xml
    testdata.path = .
    DEPLOYMENT += testdata
}

requires(contains(QT_CONFIG,private_tests))
