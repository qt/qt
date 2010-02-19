load(qttest_p4)
SOURCES += tst_xmlpatternsschemats.cpp \
           ../qxmlquery/TestFundament.cpp

include (../xmlpatterns.pri)

contains(QT_CONFIG,xmlpatterns) {
HEADERS += ../xmlpatternsxqts/tst_suitetest.h
SOURCES += ../xmlpatternsxqts/tst_suitetest.cpp
}

PATTERNIST_SDK = QtXmlPatternsSDK
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    symbian:    PATTERNIST_SDK = $${PATTERNIST_SDK}
    else:win32: PATTERNIST_SDK = $${PATTERNIST_SDK}d
    else:       PATTERNIST_SDK = $${PATTERNIST_SDK}_debug
}

LIBS += -l$$PATTERNIST_SDK

QT += xml

INCLUDEPATH += $$QT_SOURCE_TREE/tests/auto/xmlpatternssdk/   \
               $$QT_BUILD_TREE/include/QtXmlPatterns/private \
               $$QT_SOURCE_TREE/tests/auto/xmlpatternsxqts   \
               ../xmlpatternsxqts                            \
               ../xmlpatternssdk
