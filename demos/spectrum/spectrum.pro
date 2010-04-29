include(spectrum.pri)

TEMPLATE = subdirs

# Ensure that library is built before application
CONFIG  += ordered

!contains(DEFINES, DISABLE_FFT) {
    SUBDIRS += fftreal
}

SUBDIRS += app

TARGET = spectrum

symbian {
    # Create a 'make sis' rule which can be run from the top-level

    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)

    # UID for the SIS file
    TARGET.UID3 = 0xA000E3FA

    epoc32_dir = $${EPOCROOT}epoc32
    release_dir = $${epoc32_dir}/release/$(PLATFORM)/$(TARGET)

    bin.sources     = $${release_dir}/spectrum.exe
    !contains(DEFINES, DISABLE_FFT) {
        bin.sources += $${release_dir}/fftreal.dll
    }
    bin.path        = !:/sys/bin
    rsc.sources     = $${epoc32_dir}/data/z/resource/apps/spectrum.rsc
    rsc.path        = !:/resource/apps
    mif.sources     = $${epoc32_dir}/data/z/resource/apps/spectrum.mif
    mif.path        = !:/resource/apps
    reg_rsc.sources = $${epoc32_dir}/data/z/private/10003a3f/import/apps/spectrum_reg.rsc
    reg_rsc.path    = !:/private/10003a3f/import/apps
    DEPLOYMENT += bin rsc mif reg_rsc
}
