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

