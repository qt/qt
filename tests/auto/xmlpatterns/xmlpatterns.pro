load(qttest_p4)
SOURCES += tst_xmlpatterns.cpp \
           ../qxmlquery/TestFundament.cpp

!wince* {
DEFINES += SRCDIR=\\\"$$PWD/\\\"
} else {
DEFINES += SRCDIR=\\\"./\\\"
}

include (../xmlpatterns.pri)
