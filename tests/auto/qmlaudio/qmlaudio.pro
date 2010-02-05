load(qttest_p4)
SOURCES += tst_qmlaudio.cpp

QT += multimedia
requires(contains(QT_CONFIG, multimedia))
requires(contains(QT_CONFIG, declarative))
