TEMPLATE        = lib

#VERSION         = 1.1.0

CONFIG         += plugin

INCLUDEPATH    += ../

HEADERS         = leveltemplate.h

SOURCES         = leveltemplate.cpp

TARGET          = $$qtLibraryTarget(leveltemplate)

OTHER_FILES += qml/Level.qml \
               qml/Enemy.qml \
               qml/Level.js


# SYMBIAN ---------------------------
symbian: {
# Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
load(data_caging_paths)

# EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
TARGET.EPOCALLOWDLLDATA=1
TARGET.UID3 = 0xE07dfb68

TARGET.CAPABILITY = NetworkServices \
    Location \
    ReadUserData \
    WriteUserData \
    LocalServices \
    UserEnvironment

# Defines plugin stub file into Symbian .pkg package
pluginDep.sources = leveltemplate.dll
pluginDep.path = $$QT_PLUGINS_BASE_DIR/quickhitlevels
DEPLOYMENT += pluginDep

# Graphics and Sounds for the plugin
BLD_INF_RULES.prj_exports += "gfx/enemy1.png ../winscw/c/Data/gfx/enemy1.png" \
"gfx/enemy2.png ../winscw/c/Data/gfx/enemy2.png" \
"gfx/missile2.png ../winscw/c/Data/gfx/missile2.png" \
"gfx/enemy_missile2.png ../winscw/c/Data/gfx/enemy_missile2.png" \
"gfx/ship.png ../winscw/c/Data/gfx/ship.png" \
"gfx/transparent.png ../winscw/c/Data/gfx/transparent.png" \
"gfx/blue_fire.png ../winscw/c/Data/gfx/blue_fire.png" \
"sound/enemy_explosion.wav ../winscw/c/Data/sound/enemy_explosion.wav" \
"sound/myship_explosion.wav ../winscw/c/Data/sound/myship_explosion.wav" \
"sound/enableship.wav ../winscw/c/Data/sound/enableship.wav" \
"sound/laser.wav ../winscw/c/Data/sound/laser.wav" \
"sound/level2.wav ../winscw/c/Data/sound/level2.wav" \
"qml/Level.qml ../winscw/c/Data/Level.qml" \
"qml/Level.js ../winscw/c/Data/Level.js" \
"qml/Enemy.qml ../winscw/c/Data/Enemy.qml"


myQml.sources = qml/*
myQml.path = c:/system/quickhitdata/leveltemplate
myGraphic.sources = gfx/*
myGraphic.path = c:/system/quickhitdata/leveltemplate/gfx
mySound.sources = sound/*
mySound.path = c:/system/quickhitdata/leveltemplate/sound
# Takes qml, graphics and sounds into Symbian SIS package file (.pkg)
DEPLOYMENT += myQml myGraphic mySound

target.path += $$[QT_INSTALL_PLUGINS]/quickhitlevels
# Installs binaries
INSTALLS += target
}

# WINDOWS ---------------------------
win32: {
# Copy level sounds and graphics into right destination
system(mkdir c:\quickhitdata\leveltemplate)
system(mkdir c:\quickhitdata\leveltemplate\gfx)
system(mkdir c:\quickhitdata\leveltemplate\sound)
system(copy qml\*.* c:\quickhitdata\leveltemplate)
system(copy gfx\*.* c:\quickhitdata\leveltemplate\gfx)
system(copy sound\*.* c:\quickhitdata\leveltemplate\sound)

# Installs binaries
target.path += $$[QT_INSTALL_PLUGINS]/quickhitlevels
INSTALLS += target
}

# MAEMO_5 ---------------------------
maemo5: {
myQml.path = /home/user/.quickhitdata/leveltemplate/
myQml.files += qml/*
myGraphic.path = /home/user/.quickhitdata/leveltemplate/gfx/
myGraphic.files += gfx/*
mySound.path = /home/user/.quickhitdata/leveltemplate/sound/
mySound.files += sound/*

target.path += /usr/lib/qt4/plugins/quickhitlevels
# Installs qml, binaries, sounds and graphics
INSTALLS += target myGraphic mySound myQml
}
