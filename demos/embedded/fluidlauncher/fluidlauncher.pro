TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .

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

    executables.sources = \
        $$QT_BUILD_TREE/demos/embedded/embeddedsvgviewer/$${BUILD_DIR}/embeddedsvgviewer.exe \
        $$QT_BUILD_TREE/demos/embedded/styledemo/$${BUILD_DIR}/styledemo.exe \
        $$QT_BUILD_TREE/demos/deform/$${BUILD_DIR}/deform.exe \
        $$QT_BUILD_TREE/demos/pathstroke/$${BUILD_DIR}/pathstroke.exe \
        $$QT_BUILD_TREE/examples/graphicsview/elasticnodes/$${BUILD_DIR}/elasticnodes.exe \
        $$QT_BUILD_TREE/examples/widgets/wiggly/$${BUILD_DIR}/wiggly.exe \
        $$QT_BUILD_TREE/examples/painting/concentriccircles/$${BUILD_DIR}/concentriccircles.exe \
        $$QT_BUILD_TREE/examples/draganddrop/$${BUILD_DIR}/fridgemagnets.exe

    executables.path = .

    files.sources = $$PWD/screenshots $$PWD/slides $$PWD/../embeddedsvgviewer/shapes.svg
    files.path = .

    config.sources = $$PWD/config_wince/config.xml
    config.path = .

    DEPLOYMENT += config files executables

    DEPLOYMENT_PLUGIN += qgif qjpeg qmng qsvg
}

symbian {
    load(data_caging_paths)

    TARGET.UID3 = 0xA000A641
    ICON = $$QT_SOURCE_TREE/src/s60installs/qt.svg

    executables.sources = \
        styledemo.exe \
        deform.exe \
        pathstroke.exe \
        wiggly.exe \
        ftp.exe \
        saxbookmarks.exe \
        desktopservices.exe \
        fridgemagnets.exe \
        drilldown.exe \
        softkeys.exe \
        raycasting.exe \
        flickable.exe \
        digiflip.exe \
        lightmaps.exe \
        flightinfo.exe

    executables.path = /sys/bin

    reg_resource.sources = \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/styledemo_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/deform_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/pathstroke_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/wiggly_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/ftp_reg.rsc\
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/saxbookmarks_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/desktopservices_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/fridgemagnets_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/drilldown_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/softkeys_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/raycasting_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/flickable_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/digiflip_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/lightmaps_reg.rsc \
        $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/flightinfo_reg.rsc

    contains(QT_CONFIG, phonon) {
        reg_resource.sources += $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/qmediaplayer_reg.rsc
    }


    reg_resource.path = $$REG_RESOURCE_IMPORT_DIR

    resource.sources = \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/styledemo.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/deform.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/pathstroke.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/wiggly.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/ftp.rsc\
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/saxbookmarks.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/desktopservices.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/fridgemagnets.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/drilldown.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/softkeys.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/raycasting.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/flickable.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/digiflip.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/lightmaps.rsc \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/flightinfo.rsc

    
    resource.path = $$APP_RESOURCE_DIR

    mifs.sources = \
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000A641.mif \ #fluidlauncher
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000A63F.mif \ #styledemo
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000A63D.mif \ #deform
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000A63E.mif \ #pathstroke
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C607.mif \ #wiggly
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000A648.mif \ #ftp
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C60A.mif \ #saxbookmarks
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C611.mif \ #desktopservices
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C610.mif \ #fridgemagnets
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C612.mif \ #drilldown
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF6B.mif \ #softkeys
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF76.mif \ #raycasting
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF73.mif \ #flickable
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF72.mif \ #digiflip
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF75.mif \ #lightmaps
        $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF74.mif   #flightinfo
    mifs.path = $$APP_RESOURCE_DIR

    contains(QT_CONFIG, svg) { 
        executables.sources += \ 
            embeddedsvgviewer.exe \
            weatherinfo.exe
            
        reg_resource.sources += \
            $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/embeddedsvgviewer_reg.rsc \
            $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/weatherinfo_reg.rsc
            
        resource.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/embeddedsvgviewer.rsc \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/weatherinfo.rsc

        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000A640.mif \ #embeddedsvgviewer
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF77.mif   #weatherinfo

    }
    contains(QT_CONFIG, webkit) {
        executables.sources += anomaly.exe
        reg_resource.sources += $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/anomaly_reg.rsc
        resource.sources += $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/anomaly.rsc
        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000CF71.mif   #anomaly
    }

    contains(QT_CONFIG, phonon) {
        executables.sources += qmediaplayer.exe
        resource.sources += $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/qmediaplayer.rsc
        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C613.mif   #qmediaplayer
    }

    contains(QT_CONFIG, script) {
        executables.sources += context2d.exe    
        reg_resource.sources += $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/context2d_reg.rsc
        resource.sources += $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/context2d.rsc
        mifs.sources += \
            $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/0xA000C608.mif   #context2d
    }

    files.sources = $$PWD/screenshots $$PWD/slides
    files.path = .

    config.sources = $$PWD/config_s60/config.xml
    config.path = .

    viewerimages.sources = $$PWD/../embeddedsvgviewer/shapes.svg
    viewerimages.path = /data/images/qt/demos/embeddedsvgviewer

    # demos/mediaplayer make also use of these files.
    desktopservices_music.sources = \
        $$PWD/../desktopservices/data/*.mp3 \
        $$PWD/../desktopservices/data/*.wav
    desktopservices_music.path = /data/sounds

    desktopservices_images.sources = $$PWD/../desktopservices/data/*.png
    desktopservices_images.path = /data/images

    saxbookmarks.sources = $$PWD/../../../examples/xml/saxbookmarks/frank.xbel
    saxbookmarks.sources += $$PWD/../../../examples/xml/saxbookmarks/jennifer.xbel
    saxbookmarks.path = /data/qt/saxbookmarks

    DEPLOYMENT += config files executables viewerimages saxbookmarks reg_resource resource \
        mifs desktopservices_music desktopservices_images

    TARGET.EPOCHEAPSIZE = 100000 20000000
}
