include(spectrum.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered

!contains(DEFINES, DISABLE_FFT) {
    SUBDIRS += 3rdparty/fftreal
}

SUBDIRS += app

TARGET = spectrum

sources.files = README.txt spectrum.pri spectrum.pro TODO.txt
sources.path = $$[QT_INSTALL_DEMOS]/spectrum
INSTALLS += sources

