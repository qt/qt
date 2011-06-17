# checksum 0xc123 version 0x10008
# This file should not be edited.
# Future versions of Qt Creator might offer updated versions of this file.

QT += declarative

SOURCES += $$PWD/qmlapplicationviewer.cpp
HEADERS += $$PWD/qmlapplicationviewer.h
INCLUDEPATH += $$PWD

contains(DEFINES, QMLOBSERVER) {
    DEFINES *= QMLJSDEBUGGER
}

defineTest(minQtVersion) {
    maj = $$1
    min = $$2
    patch = $$3
    isEqual(QT_MAJOR_VERSION, $$maj) {
        isEqual(QT_MINOR_VERSION, $$min) {
            isEqual(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
            greaterThan(QT_PATCH_VERSION, $$patch) {
                return(true)
            }
        }
        greaterThan(QT_MINOR_VERSION, $$min) {
            return(true)
        }
    }
    return(false)
}

contains(DEFINES, QMLJSDEBUGGER) {
    CONFIG(debug, debug|release) {
        !minQtVersion(4, 7, 1) {
            warning()
            warning("Debugging QML requires the qmljsdebugger library that ships with Qt Creator.")
            warning("This library requires Qt 4.7.1 or newer.")
            warning()

            error("Qt version $$QT_VERSION too old for QmlJS Debugging. Aborting.")
        }
        isEmpty(QMLJSDEBUGGER_PATH) {
            warning()
            warning("Debugging QML requires the qmljsdebugger library that ships with Qt Creator.")
            warning("Please specify its location on the qmake command line, eg")
            warning("  qmake -r QMLJSDEBUGGER_PATH=$CREATORDIR/share/qtcreator/qmljsdebugger")
            warning()

            error("QMLJSDEBUGGER defined, but no QMLJSDEBUGGER_PATH set on command line. Aborting.")
            DEFINES -= QMLJSDEBUGGER
        } else {
            include($$QMLJSDEBUGGER_PATH/qmljsdebugger-lib.pri)
        }
    } else {
        DEFINES -= QMLJSDEBUGGER
    }
}
# This file should not be edited.
# Future versions of Qt Creator might offer updated versions of this file.

defineTest(qtcAddDeployment) {
for(deploymentfolder, DEPLOYMENTFOLDERS) {
    item = item$${deploymentfolder}
    itemsources = $${item}.sources
    $$itemsources = $$eval($${deploymentfolder}.source)
    itempath = $${item}.path
    $$itempath= $$eval($${deploymentfolder}.target)
    export($$itemsources)
    export($$itempath)
    DEPLOYMENT += $$item
}

MAINPROFILEPWD = $$PWD

symbian {
    ICON = $${TARGET}.svg
    TARGET.EPOCHEAPSIZE = 0x20000 0x2000000
    contains(DEFINES, ORIENTATIONLOCK):LIBS += -lavkon -leikcore -leiksrv -lcone
    contains(DEFINES, NETWORKACCESS):TARGET.CAPABILITY += NetworkServices
} else:win32 {
    !isEqual(PWD,$$OUT_PWD) {
        copyCommand = @echo Copying application data...
        for(deploymentfolder, DEPLOYMENTFOLDERS) {
            source = $$eval($${deploymentfolder}.source)
            pathSegments = $$split(source, /)
            sourceAndTarget = $$MAINPROFILEPWD/$$source $$OUT_PWD/$$eval($${deploymentfolder}.target)/$$last(pathSegments)
            copyCommand += && $(COPY_DIR) $$replace(sourceAndTarget, /, \\)
        }
        copydeploymentfolders.commands = $$copyCommand
        first.depends = $(first) copydeploymentfolders
        export(first.depends)
        export(copydeploymentfolders.commands)
        QMAKE_EXTRA_TARGETS += first copydeploymentfolders
    }
} else:unix {
    maemo5 {
        installPrefix = /opt/usr
        desktopfile.path = /usr/share/applications/hildon
    } else {
        installPrefix = /usr/local
        desktopfile.path = /usr/share/applications
        !isEqual(PWD,$$OUT_PWD) {
            copyCommand = @echo Copying application data...
            for(deploymentfolder, DEPLOYMENTFOLDERS) {
                macx {
                    target = $$OUT_PWD/$${TARGET}.app/Contents/Resources/$$eval($${deploymentfolder}.target)
                } else {
                    target = $$OUT_PWD/$$eval($${deploymentfolder}.target)
                }
                copyCommand += && $(MKDIR) $$target
                copyCommand += && $(COPY_DIR) $$MAINPROFILEPWD/$$eval($${deploymentfolder}.source) $$target
            }
            copydeploymentfolders.commands = $$copyCommand
            first.depends = $(first) copydeploymentfolders
            export(first.depends)
            export(copydeploymentfolders.commands)
            QMAKE_EXTRA_TARGETS += first copydeploymentfolders
        }
    }
    for(deploymentfolder, DEPLOYMENTFOLDERS) {
        item = item$${deploymentfolder}
        itemfiles = $${item}.files
        $$itemfiles = $$eval($${deploymentfolder}.source)
        itempath = $${item}.path
        $$itempath = $${installPrefix}/share/$${TARGET}/$$eval($${deploymentfolder}.target)
        export($$itemfiles)
        export($$itempath)
        INSTALLS += $$item
    }
    icon.files = $${TARGET}.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    desktopfile.files = $${TARGET}.desktop
    target.path = $${installPrefix}/bin
    export(icon.files)
    export(icon.path)
    export(desktopfile.files)
    export(desktopfile.path)
    export(target.path)
    INSTALLS += desktopfile icon target
}

export (ICON)
export (INSTALLS)
export (DEPLOYMENT)
export (TARGET.EPOCHEAPSIZE)
export (TARGET.CAPABILITY)
export (LIBS)
export (QMAKE_EXTRA_TARGETS)
}
