TEMPLATE        = lib

#VERSION         = 1.1.0

CONFIG         += plugin

INCLUDEPATH    += ../

HEADERS         = levelone.h

SOURCES         = levelone.cpp

TARGET          = $$qtLibraryTarget(levelone)

OTHER_FILES += Level.qml


# SYMBIAN ---------------------------
symbian: {
# Load predefined include paths (e.g. QT_PLUGINS_BASE_DIR) to be used in the pro-files
load(data_caging_paths)

# EPOCALLOWDLLDATA have to set true because Qt macros has initialised global data
TARGET.EPOCALLOWDLLDATA=1
TARGET.UID3 = 0xE07dfb66

TARGET.CAPABILITY = NetworkServices \
    Location \
    ReadUserData \
    WriteUserData \
    LocalServices \
    UserEnvironment

# Defines plugin stub file into Symbian .pkg package
pluginDep.sources = levelone.dll
pluginDep.path = $$QT_PLUGINS_BASE_DIR/quickhitlevels
DEPLOYMENT += pluginDep

# Graphics and Sounds for the plugin
BLD_INF_RULES.prj_exports += "gfx/background3.png ../winscw/c/Data/gfx/background3.png" \
"gfx/enemy1.png ../winscw/c/Data/gfx/enemy1.png" \
"gfx/enemy2.png ../winscw/c/Data/gfx/enemy2.png" \
"gfx/red_fire.png ../winscw/c/Data/gfx/red_fire.png" \
"gfx/missile2.png ../winscw/c/Data/gfx/missile2.png" \
"gfx/enemy_missile2.png ../winscw/c/Data/gfx/enemy_missile2.png" \
"gfx/ship.png ../winscw/c/Data/gfx/ship.png" \
"gfx/transparent.png ../winscw/c/Data/gfx/transparent.png" \
"gfx/star3.png ../winscw/c/Data/gfx/star3.png" \
"sound/crash.wav ../winscw/c/Data/sound/crash.wav" \
"sound/rocket.wav ../winscw/c/Data/sound/rocket.wav" \
"sound/rocket_explosion.wav ../winscw/c/Data/sound/rocket_explosion.wav" \
"sound/levelonestart.wav ../winscw/c/Data/sound/levelonestart.wav" \
"Level.qml ../winscw/c/Data/Level.qml"


myQml.sources = level.qml
myQml.path = c:/system/quickhitdata/levelone
myGraphic.sources = gfx/*
myGraphic.path = c:/system/quickhitdata/levelone/gfx
mySound.sources = sound/*
mySound.path = c:/system/quickhitdata/levelone/sound
# Takes qml, graphics and sounds into Symbian SIS package file (.pkg)
DEPLOYMENT += myQml myGraphic mySound

target.path += $$[QT_INSTALL_PLUGINS]/quickhitlevels
# Installs binaries
INSTALLS += target
}

# WINDOWS ---------------------------
win32: {
# Copy level sounds and graphics into right destination
system(mkdir c:\\quickhitdata\\levelone)
system(mkdir c:\\quickhitdata\\levelone\\gfx)
system(mkdir c:\\quickhitdata\\levelone\\sound)
system(copy *.qml c:\\quickhitdata\\levelone)
system(copy gfx\\*.* c:\\quickhitdata\\levelone\\gfx)
system(copy sound\\*.* c:\\quickhitdata\\levelone\\sound)

# Installs binaries
target.path += $$[QT_INSTALL_PLUGINS]/quickhitlevels
INSTALLS += target
}

# MAEMO_5 ---------------------------
maemo5: {
myQml.path = /home/user/.quickhitdata/levelone/
myQml.files += Level.qml
myGraphic.path = /home/user/.quickhitdata/levelone/gfx/
myGraphic.files += gfx/*
mySound.path = /home/user/.quickhitdata/levelone/sound/
mySound.files += sound/*

target.path += /usr/lib/qt4/plugins/quickhitlevels
# Installs qml, binaries, sounds and graphics
INSTALLS += target myGraphic mySound myQml
}
