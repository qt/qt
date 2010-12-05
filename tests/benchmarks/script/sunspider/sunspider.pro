load(qttest_p4)
TEMPLATE = app
TARGET = tst_bench_sunspider

SOURCES += tst_sunspider.cpp

QT = core script

!symbian:DEFINES += SRCDIR=\\\"$$PWD\\\"

wince*|symbian: {
testFiles.files = tests
testFiles.path = .
DEPLOYMENT += testFiles
}

symbian* {
    TARGET.EPOCHEAPSIZE  = 0x20000 0x2000000 // Min 128kB, Max 32MB
    TARGET.EPOCSTACKSIZE = 0x14000
}
