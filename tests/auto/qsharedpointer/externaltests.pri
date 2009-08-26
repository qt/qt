SOURCES += $$PWD/externaltests.cpp
HEADERS += $$PWD/externaltests.h
cleanedQMAKESPEC = $$replace(QMAKESPEC, \\\\, /)
!symbian:DEFINES += DEFAULT_MAKESPEC=\\\"$$cleanedQMAKESPEC\\\"

embedded:DEFINES += QTEST_NO_RTTI QTEST_CROSS_COMPILED
wince*:DEFINES += QTEST_CROSS_COMPILED
symbian: DEFINES += QTEST_CROSS_COMPILED
