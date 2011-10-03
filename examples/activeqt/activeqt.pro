TEMPLATE      = subdirs
SUBDIRS      += comapp \
                hierarchy \
                menus \
                multiple \
                simple \
                webbrowser \
                wrapper

contains(QT_CONFIG, opengl):SUBDIRS += opengl

# For now only the contain examples with mingw, for the others you need
# an IDL compiler
win32-g++*|wince*:SUBDIRS = webbrowser
# Due to build problems, active qt was disabled from mingw build,
# so we have to remove the only one left
win32-g++*:SUBDIRS = 

# install
target.path = $$[QT_INSTALL_EXAMPLES]/activeqt
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS activeqt.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/activeqt
INSTALLS += target sources
