TEMPLATE=app
CONFIG -= qt separate_debug_info
LIBS =
QT =
QMAKE_LINK = @: dummy

sources.files = \
    flickr-desktop.qml \
    flickr-mobile-90.qml \
    flickr-mobile.qml \
    common \
    mobile

sources.path = $$[QT_INSTALL_DEMOS]/declarative/flickr
INSTALLS = sources

