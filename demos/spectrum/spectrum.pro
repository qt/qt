load(data_caging_paths)
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
    TARGET.UID3 = 0xA000E3FA

    bin.sources     = spectrum.exe
    !contains(DEFINES, DISABLE_FFT) {
        bin.sources += fftreal.dll
    }
    bin.path        = /sys/bin
    rsc.sources     = $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/spectrum.rsc
    rsc.path        = $$APP_RESOURCE_DIR
    mif.sources     = $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/spectrum.mif
    mif.path        = $$APP_RESOURCE_DIR
    reg_rsc.sources = $${EPOCROOT}$$HW_ZDIR$$REG_RESOURCE_IMPORT_DIR/spectrum_reg.rsc
    reg_rsc.path    = $$REG_RESOURCE_IMPORT_DIR
    DEPLOYMENT += bin rsc mif reg_rsc
}
