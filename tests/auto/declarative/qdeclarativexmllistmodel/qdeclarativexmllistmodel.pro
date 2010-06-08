load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative script gui
contains(QT_CONFIG,xmlpatterns) {
    QT += xmlpatterns
    DEFINES += QTEST_XMLPATTERNS
}
macx:CONFIG -= app_bundle

SOURCES += tst_qdeclarativexmllistmodel.cpp

symbian: {
    importFiles.sources = data
    importFiles.path = .
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

CONFIG += parallel_test

