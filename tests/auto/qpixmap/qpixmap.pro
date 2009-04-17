load(qttest_p4)
SOURCES  += tst_qpixmap.cpp
contains(QT_CONFIG, qt3support): QT += qt3support
wince*: {
   task31722_0.sources = convertFromImage/task31722_0/*
   task31722_0.path    = convertFromImage/task31722_0
   task31722_1.sources = convertFromImage/task31722_1/*
   task31722_1.path    = convertFromImage/task31722_1
   DEPLOYMENT += task31722_0 task31722_1
   DEFINES += SRCDIR=\\\".\\\"
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
   win32:LIBS += -lgdi32 -luser32
}



