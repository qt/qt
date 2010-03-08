TEMPLATE=app
CONFIG -= qt separate_debug_info
LIBS =
QT =
QMAKE_LINK = @: dummy

sources.files = \
    content \
    snake.qml

sources.path = $$[QT_INSTALL_DEMOS]/declarative/snake
INSTALLS = sources

