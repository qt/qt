TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
VERSION = $$QT_VERSION

# Input
HEADERS += \
           demoapplication.h \
           fluidlauncher.h \
           pictureflow.h \
           slideshow.h

SOURCES += \
           demoapplication.cpp \
           fluidlauncher.cpp \
           main.cpp \
           pictureflow.cpp \
           slideshow.cpp

embedded{
    target.path = $$[QT_INSTALL_DEMOS]/embedded/fluidlauncher
    sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.html config.xml screenshots slides
    sources.path = $$[QT_INSTALL_DEMOS]/embedded/fluidlauncher
    INSTALLS += target sources
}

wince*{
    QT += svg

    BUILD_DIR = release
    if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
        BUILD_DIR = debug
    }

    executables.files = \
        $$QT_BUILD_TREE/demos/embedded/embeddedsvgviewer/$${BUILD_DIR}/embeddedsvgviewer.exe \
        $$QT_BUILD_TREE/demos/embedded/styledemo/$${BUILD_DIR}/styledemo.exe \
        $$QT_BUILD_TREE/demos/deform/$${BUILD_DIR}/deform.exe \
        $$QT_BUILD_TREE/demos/pathstroke/$${BUILD_DIR}/pathstroke.exe \
        $$QT_BUILD_TREE/examples/graphicsview/elasticnodes/$${BUILD_DIR}/elasticnodes.exe \
        $$QT_BUILD_TREE/examples/widgets/wiggly/$${BUILD_DIR}/wiggly.exe \
        $$QT_BUILD_TREE/examples/painting/concentriccircles/$${BUILD_DIR}/concentriccircles.exe \
        $$QT_BUILD_TREE/examples/draganddrop/$${BUILD_DIR}/fridgemagnets.exe

    executables.path = .

    files.files = $$PWD/screenshots $$PWD/slides $$PWD/../embeddedsvgviewer/shapes.svg
    files.path = .

    config.files = $$PWD/config_wince/config.xml
    config.path = .

    DEPLOYMENT += config files executables

    DEPLOYMENT_PLUGIN += qgif qjpeg qmng qsvg
}

symbian {
    load(data_caging_paths)
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)
    RSS_RULES = # Clear RSS_RULES, otherwise fluidlauncher will get put into QtDemos folder

    TARGET.UID3 = 0xA000A641

    defineReplace(regResourceDir) {
        symbian-abld|symbian-sbsv2 {
            return($${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/$$basename(1))
        } else {
            return($${QT_BUILD_TREE}/$$1)
        }
    }

    defineReplace(appResourceDir) {
        symbian-abld|symbian-sbsv2 {
            return($${EPOCROOT}$${HW_ZDIR}$${APP_RESOURCE_DIR}/$$basename(1))
        } else {
            return($${QT_BUILD_TREE}/$$1)
        }
    }

    executables.files = \
        $$QT_BUILD_TREE/demos/embedded/styledemo/styledemo.exe \
        $$QT_BUILD_TREE/demos/deform/deform.exe \
        $$QT_BUILD_TREE/demos/pathstroke/pathstroke.exe \
        $$QT_BUILD_TREE/examples/widgets/wiggly/wiggly.exe \
        $$QT_BUILD_TREE/examples/network/qftp/qftp.exe \
        $$QT_BUILD_TREE/examples/xml/saxbookmarks/saxbookmarks.exe \
        $$QT_BUILD_TREE/demos/embedded/desktopservices/desktopservices.exe \
        $$QT_BUILD_TREE/examples/draganddrop/fridgemagnets/fridgemagnets.exe \
        $$QT_BUILD_TREE/examples/widgets/softkeys/softkeys.exe \
        $$QT_BUILD_TREE/demos/embedded/raycasting/raycasting.exe \
        $$QT_BUILD_TREE/demos/embedded/flickable/flickable.exe \
        $$QT_BUILD_TREE/demos/embedded/digiflip/digiflip.exe \
        $$QT_BUILD_TREE/demos/embedded/lightmaps/lightmaps.exe \
        $$QT_BUILD_TREE/demos/embedded/flightinfo/flightinfo.exe

    executables.path = /sys/bin

    reg_resource.files = \
        $$regResourceDir(demos/embedded/styledemo/styledemo_reg.rsc) \
        $$regResourceDir(demos/deform/deform_reg.rsc) \
        $$regResourceDir(demos/pathstroke/pathstroke_reg.rsc) \
        $$regResourceDir(examples/widgets/wiggly/wiggly_reg.rsc) \
        $$regResourceDir(examples/network/qftp/qftp_reg.rsc)\
        $$regResourceDir(examples/xml/saxbookmarks/saxbookmarks_reg.rsc) \
        $$regResourceDir(demos/embedded/desktopservices/desktopservices_reg.rsc) \
        $$regResourceDir(examples/draganddrop/fridgemagnets/fridgemagnets_reg.rsc) \
        $$regResourceDir(examples/widgets/softkeys/softkeys_reg.rsc) \
        $$regResourceDir(demos/embedded/raycasting/raycasting_reg.rsc) \
        $$regResourceDir(demos/embedded/flickable/flickable_reg.rsc) \
        $$regResourceDir(demos/embedded/digiflip/digiflip_reg.rsc) \
        $$regResourceDir(demos/embedded/lightmaps/lightmaps_reg.rsc) \
        $$regResourceDir(demos/embedded/flightinfo/flightinfo_reg.rsc)

    contains(QT_CONFIG, phonon) {
        reg_resource.files += $$regResourceDir(demos/qmediaplayer/qmediaplayer_reg.rsc)
    }

    contains(QT_CONFIG, multimedia) {
        reg_resource.files += $$regResourceDir(demos/spectrum/app/spectrum_reg.rsc)
    }


    reg_resource.path = $$REG_RESOURCE_IMPORT_DIR

    resource.files = \
        $$appResourceDir(demos/embedded/styledemo/styledemo.rsc) \
        $$appResourceDir(demos/deform/deform.rsc) \
        $$appResourceDir(demos/pathstroke/pathstroke.rsc) \
        $$appResourceDir(examples/widgets/wiggly/wiggly.rsc) \
        $$appResourceDir(examples/network/qftp/qftp.rsc)\
        $$appResourceDir(examples/xml/saxbookmarks/saxbookmarks.rsc) \
        $$appResourceDir(demos/embedded/desktopservices/desktopservices.rsc) \
        $$appResourceDir(examples/draganddrop/fridgemagnets/fridgemagnets.rsc) \
        $$appResourceDir(examples/widgets/softkeys/softkeys.rsc) \
        $$appResourceDir(demos/embedded/raycasting/raycasting.rsc) \
        $$appResourceDir(demos/embedded/flickable/flickable.rsc) \
        $$appResourceDir(demos/embedded/digiflip/digiflip.rsc) \
        $$appResourceDir(demos/embedded/lightmaps/lightmaps.rsc) \
        $$appResourceDir(demos/embedded/flightinfo/flightinfo.rsc)


    resource.path = $$APP_RESOURCE_DIR

    mifs.files = \
        $$appResourceDir(demos/embedded/fluidlauncher/fluidlauncher.mif) \
        $$appResourceDir(demos/embedded/styledemo/styledemo.mif) \
        $$appResourceDir(demos/deform/deform.mif) \
        $$appResourceDir(demos/pathstroke/pathstroke.mif) \
        $$appResourceDir(examples/widgets/wiggly/wiggly.mif) \
        $$appResourceDir(examples/network/qftp/qftp.mif) \
        $$appResourceDir(examples/xml/saxbookmarks/saxbookmarks.mif) \
        $$appResourceDir(demos/embedded/desktopservices/desktopservices.mif) \
        $$appResourceDir(examples/draganddrop/fridgemagnets/fridgemagnets.mif) \
        $$appResourceDir(examples/widgets/softkeys/softkeys.mif) \
        $$appResourceDir(demos/embedded/raycasting/raycasting.mif) \
        $$appResourceDir(demos/embedded/flickable/flickable.mif) \
        $$appResourceDir(demos/embedded/digiflip/digiflip.mif) \
        $$appResourceDir(demos/embedded/lightmaps/lightmaps.mif) \
        $$appResourceDir(demos/embedded/flightinfo/flightinfo.mif)
    mifs.path = $$APP_RESOURCE_DIR

    contains(QT_CONFIG, svg) {
        executables.files += \
            $$QT_BUILD_TREE/demos/embedded/embeddedsvgviewer/embeddedsvgviewer.exe \
            $$QT_BUILD_TREE/demos/embedded/weatherinfo/weatherinfo.exe

        reg_resource.files += \
            $$regResourceDir(demos/embedded/embeddedsvgviewer/embeddedsvgviewer_reg.rsc) \
            $$regResourceDir(demos/embedded/weatherinfo/weatherinfo_reg.rsc)

        resource.files += \
            $$appResourceDir(demos/embedded/embeddedsvgviewer/embeddedsvgviewer.rsc) \
            $$appResourceDir(demos/embedded/weatherinfo/weatherinfo.rsc)

        mifs.files += \
            $$appResourceDir(demos/embedded/embeddedsvgviewer/embeddedsvgviewer.mif) \
            $$appResourceDir(demos/embedded/weatherinfo/weatherinfo.mif)

    }
    contains(QT_CONFIG, webkit) {
        executables.files += $$QT_BUILD_TREE/demos/embedded/anomaly/anomaly.exe
        reg_resource.files += $$regResourceDir(demos/embedded/anomaly/anomaly_reg.rsc)
        resource.files += $$appResourceDir(demos/embedded/anomaly/anomaly.rsc)
        mifs.files += \
            $$appResourceDir(demos/embedded/anomaly/anomaly.mif)

        isEmpty(QT_LIBINFIX) {
            # Since Fluidlauncher itself doesn't link webkit, we won't get dependency automatically
            executables.pkg_prerules += \
                "; Dependency to Qt Webkit" \
                "(0x200267C2), $${QT_MAJOR_VERSION}, $${QT_MINOR_VERSION}, $${QT_PATCH_VERSION},  {\"QtWebKit\"}"
        }
    }

    contains(QT_CONFIG, phonon) {
        executables.files += $$QT_BUILD_TREE/demos/qmediaplayer/qmediaplayer.exe
        resource.files += $$appResourceDir(demos/qmediaplayer/qmediaplayer.rsc)
        mifs.files += \
            $$appResourceDir(demos/qmediaplayer/qmediaplayer.mif)
    }

    contains(QT_CONFIG, multimedia) {
        executables.files += $$QT_BUILD_TREE/demos/spectrum/app/spectrum.exe
        executables.files += $$QT_BUILD_TREE/demos/spectrum/3rdparty/fftreal/fftreal.dll
        resource.files += $$appResourceDir(demos/spectrum/app/spectrum.rsc)
        mifs.files += \
            $$appResourceDir(demos/spectrum/app/spectrum.mif)
    }

    contains(QT_CONFIG, script) {
        executables.files += $$QT_BUILD_TREE/examples/script/context2d/context2d.exe
        reg_resource.files += $$regResourceDir(examples/script/context2d/context2d_reg.rsc)
        resource.files += $$appResourceDir(examples/script/context2d/context2d.rsc)
        mifs.files += \
            $$appResourceDir(examples/script/context2d/context2d.mif)
    }

    qmldemos = qmlcalculator qmlclocks qmldialcontrol qmleasing qmlflickr qmlphotoviewer qmltwitter
    contains(QT_CONFIG, declarative) {
        for(qmldemo, qmldemos) {
            executables.files += $$QT_BUILD_TREE/demos/embedded/$${qmldemo}/$${qmldemo}.exe
            reg_resource.files += $$regResourceDir(demos/embedded/$${qmldemo}/$${qmldemo}_reg.rsc)
            resource.files += $$appResourceDir(demos/embedded/$${qmldemo}/$${qmldemo}.rsc)
            mifs.files += $$appResourceDir(demos/embedded/$${qmldemo}/$${qmldemo}.mif)
        }
    }

    files.files = $$PWD/screenshots $$PWD/slides
    files.path = .

    config.files = $$PWD/config_s60/config.xml
    config.path = .

    viewerimages.files = $$PWD/../embeddedsvgviewer/shapes.svg
    viewerimages.path = /data/images/qt/demos/embeddedsvgviewer

    # demos/mediaplayer make also use of these files.
    desktopservices_music.files = \
        $$PWD/../desktopservices/data/*.mp3 \
        $$PWD/../desktopservices/data/*.wav
    desktopservices_music.path = /data/sounds

    desktopservices_images.files = $$PWD/../desktopservices/data/*.png
    desktopservices_images.path = /data/images

    saxbookmarks.files = $$PWD/../../../examples/xml/saxbookmarks/frank.xbel
    saxbookmarks.files += $$PWD/../../../examples/xml/saxbookmarks/jennifer.xbel
    saxbookmarks.path = /data/qt/saxbookmarks

    fluidbackup.files = backup_registration.xml
    fluidbackup.path = /private/$$replace(TARGET.UID3, 0x,)

    DEPLOYMENT += config files executables viewerimages saxbookmarks reg_resource resource \
        mifs desktopservices_music desktopservices_images fluidbackup

    contains(QT_CONFIG, declarative):for(qmldemo, qmldemos):include($$QT_BUILD_TREE/demos/embedded/$${qmldemo}/deployment.pri)

    DEPLOYMENT.installer_header = 0xA000D7CD

    TARGET.EPOCHEAPSIZE = 100000 20000000
}
