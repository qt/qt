load(qttest_p4)
SOURCES  += tst_qfileinfo.cpp

QT = core


RESOURCES      += qfileinfo.qrc

wince*:|symbian: {
    deploy.sources += qfileinfo.qrc tst_qfileinfo.cpp
    res.sources = resources\\file1 resources\\file1.ext1 resources\\file1.ext1.ext2
    res.path = resources
    DEPLOYMENT = deploy res
}

symbian {
    TARGET.CAPABILITY=AllFiles
    LIBS *= -lefsrv
    }

# support for running test from shadow build directory
wince* {
    DEFINES += SRCDIR=\\\"\\\"
} else:symbian {
    # do not define SRCDIR at all
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

