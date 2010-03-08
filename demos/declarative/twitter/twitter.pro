TEMPLATE=app
CONFIG -= qt separate_debug_info
LIBS =
QT =
QMAKE_LINK = @: dummy

sources.files = \
    TwitterCore \
    twitter.qml 

sources.path = $$[QT_INSTALL_DEMOS]/declarative/twitter
INSTALLS = sources

