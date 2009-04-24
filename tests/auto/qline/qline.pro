load(qttest_p4)
SOURCES += tst_qline.cpp
QT -= gui
unix:!mac:!symbian*:LIBS+=-lm
