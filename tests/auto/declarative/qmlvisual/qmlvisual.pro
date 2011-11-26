load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_qmlvisual.cpp

symbian: {
    importFiles.path = .
    importFiles.files = animation \
    fillmode \
    focusscope \
    ListView \
    qdeclarativeborderimage \
    qdeclarativeflickable \
    qdeclarativeflipable \
    qdeclarativegridview \
    qdeclarativemousearea \
    qdeclarativeparticles \
    qdeclarativepathview \
    qdeclarativepositioners \
    qdeclarativesmoothedanimation \
    qdeclarativespringanimation \
    qdeclarativetext \
    qdeclarativetextedit \
    qdeclarativetextinput \
    rect \
    repeater \
    selftest_noimages \
    webview
    DEPLOYMENT += importFiles
} else {
    DEFINES += QT_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
}

CONFIG += parallel_test

CONFIG+=insignificant_test # QTQAINFRA-428
