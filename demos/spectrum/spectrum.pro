include(spectrum.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered

!contains(DEFINES, DISABLE_FFT) {
    SUBDIRS += 3rdparty/fftreal
}

SUBDIRS += app

TARGET = spectrum

symbian {
    # Create a 'make sis' rule which can be run from the top-level

    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)

    # UID for the SIS file
    TARGET.UID3 = 0xA000E402
}

sources.files = README.txt spectrum.pri spectrum.pro TODO.txt
sources.path = $$[QT_INSTALL_DEMOS]/spectrum
INSTALLS += sources

