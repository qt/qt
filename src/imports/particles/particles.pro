TARGET  = qmlparticlesplugin
TARGETPATH = Qt/labs/particles
include(../qimportbase.pri)

HEADERS += \
    V1/qdeclarativeparticles_p.h \
    spritestate.h \
    pluginmain.h \
    particleaffector.h \
    wanderaffector.h \
    #rockingaffector.h \
    #scalingaffector.h \
    driftaffector.h \
    particleemitter.h \
    particlesystem.h \
    trailsemitter.h \
    #spriteemitter.h \
    particle.h \
    coloredparticle.h \
    spriteparticle.h \
    spritegoalaffector.h \
    zoneaffector.h \
    frictionaffector.h \
    gravitationalsingularityaffector.h \
    killaffector.h \
    speedlimitaffector.h \
    spriteengine.h \
    gravityaffector.h \
    attractoraffector.h \
    meanderaffector.h \
    toggleaffector.h \
    spriteimage.h \
    #pairedparticle.h \
    followemitter.h \
    followaffector.h \
    swarmaffector.h \
    turbulenceaffector.h

SOURCES += \
    V1/qdeclarativeparticles.cpp \
    spritestate.cpp \
    main.cpp \
    particleaffector.cpp \
    wanderaffector.cpp \
    #rockingaffector.cpp \
    #scalingaffector.cpp \
    driftaffector.cpp \
    particleemitter.cpp \
    particlesystem.cpp \
    trailsemitter.cpp \
    #spriteemitter.cpp \
    particle.cpp \
    coloredparticle.cpp \
    spriteparticle.cpp \
    spritegoalaffector.cpp \
    zoneaffector.cpp \
    frictionaffector.cpp \
    gravitationalsingularityaffector.cpp \
    killaffector.cpp \
    speedlimitaffector.cpp \
    spriteengine.cpp \
    gravityaffector.cpp \
    attractoraffector.cpp \
    meanderaffector.cpp \
    toggleaffector.cpp \
    spriteimage.cpp \
    #pairedparticle.cpp \
    followemitter.cpp \
    followaffector.cpp \
    swarmaffector.cpp \
    turbulenceaffector.cpp

QT += declarative opengl


OTHER_FILES += \
    qmldir

RESOURCES += \
    spriteparticles.qrc

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/imports/$$TARGETPATH
target.path = $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

qmldir.files += $$PWD/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$TARGETPATH

symbian:{
    TARGET.UID3 = 0x2002131E
    
    isEmpty(DESTDIR):importFiles.files = qmlparticlesplugin$${QT_LIBINFIX}.dll qmldir
    else:importFiles.files = $$DESTDIR/qmlparticlesplugin$${QT_LIBINFIX}.dll qmldir
    importFiles.path = $$QT_IMPORTS_BASE_DIR/$$TARGETPATH
    
    DEPLOYMENT = importFiles
}

INSTALLS += target qmldir
