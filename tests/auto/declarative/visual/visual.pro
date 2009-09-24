load(qttest_p4)
contains(QT_CONFIG,declarative): QT += declarative
SOURCES += tst_visual.cpp
macx:CONFIG -= app_bundle

DEFINES += QT_TEST_SOURCE_DIR=\"\\\"$$PWD\\\"\"
