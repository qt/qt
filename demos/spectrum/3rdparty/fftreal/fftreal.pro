TEMPLATE = lib
TARGET   = fftreal

# FFTReal
HEADERS  += Array.h \
            Array.hpp \
            DynArray.h \
            DynArray.hpp \
            FFTRealFixLen.h \
            FFTRealFixLen.hpp \
            FFTRealFixLenParam.h \
            FFTRealPassDirect.h \
            FFTRealPassDirect.hpp \
            FFTRealPassInverse.h \
            FFTRealPassInverse.hpp \
            FFTRealSelect.h \
            FFTRealSelect.hpp \
            FFTRealUseTrigo.h \
            FFTRealUseTrigo.hpp \
            OscSinCos.h \
            OscSinCos.hpp \
            def.h
	    
# Wrapper used to export the required instantiation of the FFTRealFixLen template
HEADERS  += fftreal_wrapper.h
SOURCES  += fftreal_wrapper.cpp

DEFINES  += FFTREAL_LIBRARY

symbian {
    # Provide unique ID for the generated binary, required by Symbian OS
    TARGET.UID3 = 0xA000E3FB
    TARGET.CAPABILITY = UserEnvironment
} else {
    macx {
        CONFIG += lib_bundle
    } else {
        DESTDIR = ../../bin
    }
} 

# Install

sources.files = $$SOURCES $$HEADERS fftreal.pro readme.txt license.txt
sources.files += bwins/fftreal.def eabi/fftreal.def
sources.path = $$[QT_INSTALL_DEMOS]/spectrum/3rdparty/fftreal
INSTALLS += sources

