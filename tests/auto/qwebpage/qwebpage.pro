load(qttest_p4)
contains(QT_CONFIG,webkit) {
    DEFINES += QTEST_WEBKIT
    QT += webkit network
    isEmpty(QT_BUILD_TREE):QT_BUILD_TREE=$(QTDIR)
    isEmpty(QT_SOURCE_TREE):QT_SOURCE_TREE=$$fromfile($$QT_BUILD_TREE/.qmake.cache, QT_SOURCE_TREE)

    TESTDIR = $$QT_SOURCE_TREE/src/3rdparty/webkit/Source/WebKit/qt/tests/qwebpage
    VPATH += $$TESTDIR
    include($$TESTDIR/qwebpage.pro)
} else {
    SOURCES += dummy.cpp
}

CONFIG+=insignificant_test # QTQAINFRA-428
