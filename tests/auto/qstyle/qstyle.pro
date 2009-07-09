load(qttest_p4)
SOURCES  += tst_qstyle.cpp

wince*: {
   DEFINES += SRCDIR=\\\".\\\"
   addPixmap.sources = task_25863.png
   addPixmap.path = .
   DEPLOYMENT += addPixmap
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
}


