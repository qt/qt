load(qttest_p4)

SOURCES  += tst_qhostinfo.cpp

QT = core network core

wince*: {
  LIBS += ws2.lib
} else {
  win32:LIBS += -lws2_32
}


