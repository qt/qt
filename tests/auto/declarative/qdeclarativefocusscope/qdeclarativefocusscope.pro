load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_qdeclarativefocusscope.cpp
macx:CONFIG -= app_bundle

symbian: {
    DEFINES += SRCDIR=\".\"
    importFiles.sources = data
    importFiles.path = 
    DEPLOYMENT = importFiles
} else {
    DEFINES += SRCDIR=\\\"$$PWD\\\"
}

