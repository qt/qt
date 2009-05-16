contains(QT_CONFIG,xmlpatterns) {
    DEFINES += QTEST_XMLPATTERNS
    QT += xmlpatterns
}

wince*: {
    patternsdk.sources = $$QT_BUILD_TREE/lib/QtXmlPatternsSDK*.dll
    patternsdk.path = .
    basedata.sources = xmlpaternsxqts/Baseline.xml
    basedata.path = .
    DEPLOYMENT += patternsdk
    QT += network
}
QT -= gui

XMLPATTERNS_SDK = QtXmlPatternsSDK
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    win32:XMLPATTERNS_SDK = $${XMLPATTERNS_SDK}d
    else: XMLPATTERNS_SDK = $${XMLPATTERNS_SDK}_debug
}

INCLUDEPATH += \
               $$QT_SOURCE_TREE/src/xmlpatterns/acceltree \
               $$QT_SOURCE_TREE/src/xmlpatterns/api \
               $$QT_SOURCE_TREE/src/xmlpatterns/data \
               $$QT_SOURCE_TREE/src/xmlpatterns/environment \
               $$QT_SOURCE_TREE/src/xmlpatterns/expr \
               $$QT_SOURCE_TREE/src/xmlpatterns/functions \
               $$QT_SOURCE_TREE/src/xmlpatterns/iterators \
               $$QT_SOURCE_TREE/src/xmlpatterns/janitors \
               $$QT_SOURCE_TREE/src/xmlpatterns/parser \
               $$QT_SOURCE_TREE/src/xmlpatterns/projection \
               $$QT_SOURCE_TREE/src/xmlpatterns/schema \
               $$QT_SOURCE_TREE/src/xmlpatterns/type \
               $$QT_SOURCE_TREE/src/xmlpatterns/utils
