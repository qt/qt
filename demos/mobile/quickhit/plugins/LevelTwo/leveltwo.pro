TEMPLATE        = lib

#VERSION         = 1.1.0

CONFIG         += plugin

INCLUDEPATH    += ../

HEADERS         = leveltwo.h

SOURCES         = leveltwo.cpp

TARGET          = $$qtLibraryTarget(leveltwo)

OTHER_FILES += qml/Level.qml \
               qml/Enemy.qml \
               qml/Level.js


# WINDOWS ---------------------------
win32: {

# Copy level sounds and graphics into right destination
system(mkdir c:\quickhitdata\leveltwo)
system(mkdir c:\quickhitdata\leveltwo\gfx)
system(mkdir c:\quickhitdata\leveltwo\sound)
system(copy qml\*.* c:\quickhitdata\leveltwo)
system(copy gfx\*.* c:\quickhitdata\leveltwo\gfx)
system(copy sound\*.* c:\quickhitdata\leveltwo\sound)

# Installs binaries
target.path += $$[QT_INSTALL_PLUGINS]/quickhitlevels
INSTALLS += target
}

# MAEMO_5 ---------------------------
maemo5: {
myQml.path = /home/user/.quickhitdata/leveltwo/
myQml.files += qml/*
myGraphic.path = /home/user/.quickhitdata/leveltwo/gfx/
myGraphic.files += gfx/*
mySound.path = /home/user/.quickhitdata/leveltwo/sound/
mySound.files += sound/*

target.path += /usr/lib/qt4/plugins/quickhitlevels
# Installs qml, binaries, sounds and graphics
INSTALLS += target myGraphic mySound myQml
}
