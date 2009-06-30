load(qttest_p4)
QT = core
contains(QT_CONFIG,dbus): { 
    SOURCES += tst_qdbusabstractinterface.cpp interface.cpp
    HEADERS += interface.h
    QT += dbus

    # These are generated sources
    # To regenerate, see the command-line at the top of the files
    SOURCES += pinger.cpp
    HEADERS += pinger.h
}
else:SOURCES += ../qdbusmarshall/dummy.cpp

OTHER_FILES += com.trolltech.QtDBus.Pinger.xml
