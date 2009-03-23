load(qttest_p4)
SOURCES += tst_qdatastream.cpp

cross_compile: DEFINES += SVGFILE=\\\"tests2.svg\\\"
else: DEFINES += SVGFILE=\\\"gearflowers.svg\\\"

# for qpaintdevicemetrics.h
contains(QT_CONFIG, qt3support):QT += qt3support
QT += svg


wince*: {
   addFiles.sources = datastream.q42 gearflowers.svg
   addFiles.path = .
   DEPLOYMENT += addFiles
   DEFINES += SRCDIR=\\\"\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD/\\\"
}

