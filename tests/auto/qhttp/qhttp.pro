load(qttest_p4)
SOURCES  += tst_qhttp.cpp


QT = core network

wince*: {
    webFiles.sources = webserver/*
    webFiles.path = webserver
    cgi.sources = webserver/cgi-bin/*
    cgi.path = webserver/cgi-bin
    addFiles.sources = rfc3252.txt trolltech
    addFiles.path = .
    DEPLOYMENT = addFiles webFiles cgi
    DEFINES += SRCDIR=\\\"\\\"
} else:symbian {
    webFiles.sources = webserver/*
    webFiles.path = webserver
    cgi.sources = webserver/cgi-bin/*
    cgi.path = webserver/cgi-bin
    addFiles.sources = rfc3252.txt trolltech
    addFiles.path = .
    DEPLOYMENT = addFiles webFiles cgi
    TARGET.CAPABILITY = NetworkServices
} else:vxworks*: {
    DEFINES += SRCDIR=\\\"\\\"
} else {
    DEFINES += SRCDIR=\\\"$$PWD/\\\"
}
