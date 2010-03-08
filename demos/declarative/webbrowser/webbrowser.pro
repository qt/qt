TEMPLATE=app
CONFIG -= qt separate_debug_info
LIBS =
QT =
QMAKE_LINK = @: dummy

sources.files = \
    content \
    webbrowser.qml 

sources.path = $$[QT_INSTALL_DEMOS]/declarative/webbrowser
INSTALLS = sources

