SOURCES += main.cpp
TARGET = lackey

QT = core network

DESTDIR = ./

win32:CONFIG += console

symbian {
     # Needed for interprocess communication and opening QNetworkSession
     TARGET.CAPABILITY = NetworkControl NetworkServices
}

