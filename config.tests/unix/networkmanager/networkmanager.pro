SOURCES = main.cpp
CONFIG -= qt dylib
mac:CONFIG -= app_bundle

!contains(QT_CONFIG,dbus): {
    DEFINES += QT_NO_DBUS
}