SOURCES += main.cpp
TARGET = qnetworksessionlackey

QT = core network

symbian {
     # Needed for interprocess communication and opening QNetworkSession
     TARGET.CAPABILITY = NetworkControl NetworkServices
}

