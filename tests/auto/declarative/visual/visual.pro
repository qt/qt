load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
macx:CONFIG -= app_bundle

SOURCES += tst_visual.cpp

DEFINES += QT_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
