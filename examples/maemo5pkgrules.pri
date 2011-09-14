!maemo5: error(Only include this file for Maemo5 platforms)

PREFIX = /opt/usr
BINDIR = $$PREFIX/bin
DATADIR = $$PREFIX/share

provide_icon.commands = "$$QMAKE_COPY \"$$PWD/qt.png\" \"$$OUT_PWD/$${TARGET}.png\""
QMAKE_EXTRA_TARGETS += provide_icon
CLEANFILES += $$OUT_PWD/$${TARGET}.png

icon.CONFIG += no_check_exist
icon.files = $$OUT_PWD/$${TARGET}.png
icon.path = /usr/share/icons/hicolor/64x64/apps/
icon.depends = provide_icon

desktopfile.path = /usr/share/applications/hildon
desktopfile.files = $${TARGET}.desktop

DEFINES += DATADIR=\\\"$$DATADIR\\\" \
        PKGDATADIR=\\\"$$PKGDATADIR\\\"

target.path = $$BINDIR

INSTALLS += desktopfile icon

# Don't install sources on the embedded target
INSTALLS -= sources
