load(qttest_p4)
SOURCES += tst_qdeclarativeaudio.cpp

QT += multimedia declarative
requires(contains(QT_CONFIG, multimedia))
requires(contains(QT_CONFIG, declarative))
