load(qttest_p4)
SOURCES += tst_qimagereader.cpp
MOC_DIR=tmp
QT += network
RESOURCES += qimagereader.qrc
!symbian:DEFINES += SRCDIR=\\\"$$PWD\\\"

!contains(QT_CONFIG, no-gif):DEFINES += QTEST_HAVE_GIF
!contains(QT_CONFIG, no-jpeg):DEFINES += QTEST_HAVE_JPEG
!contains(QT_CONFIG, no-mng):DEFINES += QTEST_HAVE_MNG
!contains(QT_CONFIG, no-tiff):DEFINES += QTEST_HAVE_TIFF
!contains(QT_CONFIG, no-svg):DEFINES += QTEST_HAVE_SVG

win32-msvc:QMAKE_CXXFLAGS -= -Zm200
win32-msvc:QMAKE_CXXFLAGS += -Zm800
win32-msvc.net:QMAKE_CXXFLAGS -= -Zm300
win32-msvc.net:QMAKE_CXXFLAGS += -Zm1100

wince*: {
    images.sources = images
    images.path = .

    imagePlugins.sources = $$QT_BUILD_TREE/plugins/imageformats/*.dll
    imagePlugins.path = imageformats

    DEPLOYMENT += images imagePlugins
    DEFINES += SRCDIR=\\\".\\\"
}

symbian: {
    images.sources = images
    images.path = .

    DEPLOYMENT += images

    qt_not_deployed {
        imagePlugins.sources = qjpeg.dll qgif.dll qmng.dll
        imagePlugins.path = imageformats

        DEPLOYMENT += imagePlugins
    }
}
