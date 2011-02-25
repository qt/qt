load(qttest_p4)
QT       += opengl declarative

TARGET = tst_geometry
macx:CONFIG   -= app_bundle

SOURCES += tst_geometry.cpp

CONFIG+=parallel_test

