load(qttest_p4)
SOURCES  += tst_qpixmap.cpp
contains(QT_CONFIG, qt3support): QT += qt3support
wince*|symbian: {

   task31722_0.sources = convertFromImage/task31722_0/*.png
   task31722_0.path    = convertFromImage/task31722_0

   task31722_1.sources = convertFromImage/task31722_1/*.png
   task31722_1.path    = convertFromImage/task31722_1
 
   icons.sources = convertFromToHICON/*       
   icons.path = convertFromToHICON
   
   DEPLOYMENT += task31722_0 task31722_1 icons
   DEPLOYMENT_PLUGIN += qico
}

wince*: {
   DEFINES += SRCDIR=\\\".\\\"
} else:symbian {
   DEPLOYMENT_PLUGIN += qmng
   LIBS += -lfbscli.dll -lbitgdi.dll -lgdi.dll
   contains(QT_CONFIG, openvg) {
       LIBS += $$QMAKE_LIBS_OPENVG
   }
} else {
   DEFINES += SRCDIR=\\\"$$PWD\\\"
   win32:LIBS += -lgdi32 -luser32
}

RESOURCES += qpixmap.qrc
