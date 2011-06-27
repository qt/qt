TEMPLATE      = subdirs
CONFIG       += ordered
SUBDIRS       = qmusicplayer \
                capabilities

# Disable capabilities example for symbian-gcce due to a bug in elf2e32.
symbian-gcce:SUBDIRS -= capabilities

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS phonon.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon
INSTALLS += target sources

