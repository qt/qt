include(../spectrum.pri)

static: error(This application cannot be statically linked to the fftreal library)

TEMPLATE = app

TARGET = spectrum

QT       += multimedia

SOURCES  += main.cpp \
            engine.cpp \
            frequencyspectrum.cpp \
            levelmeter.cpp \
            mainwidget.cpp \
            progressbar.cpp \
            settingsdialog.cpp \
            spectrograph.cpp \
            spectrumanalyser.cpp \
            tonegenerator.cpp \
            tonegeneratordialog.cpp \
            utils.cpp \
            waveform.cpp \
            wavfile.cpp

HEADERS  += engine.h \
            frequencyspectrum.h \
            levelmeter.h \
            mainwidget.h \
            progressbar.h \
            settingsdialog.h \
            spectrograph.h \
            spectrum.h \
            spectrumanalyser.h \
            tonegenerator.h \
            tonegeneratordialog.h \
            utils.h \
            waveform.h \
            wavfile.h

fftreal_dir = ../3rdparty/fftreal

INCLUDEPATH += $${fftreal_dir}

RESOURCES = spectrum.qrc

symbian {
    # Platform security capability required to record audio on Symbian
    TARGET.CAPABILITY = UserEnvironment

    # Provide unique ID for the generated binary, required by Symbian OS
    TARGET.UID3 = 0xA000E402
}


# Dynamic linkage against FFTReal DLL
!contains(DEFINES, DISABLE_FFT) {
    symbian {
        # Must explicitly add the .dll suffix to ensure dynamic linkage
        LIBS += -lfftreal.dll
        QMAKE_LIBDIR += $${fftreal_dir}
    } else {
        macx {
            # Link to fftreal framework
            LIBS += -F$${fftreal_dir}
            LIBS += -framework fftreal
        } else {
            LIBS += -L..$${spectrum_build_dir}
            LIBS += -lfftreal
        }
    }
}

# Install

sources.files = $$SOURCES $$HEADERS $$RESOURCES app.pro
sources.path = $$[QT_INSTALL_DEMOS]/spectrum/app
images.files += images/record.png images/settings.png
images.path = $$[QT_INSTALL_DEMOS]/spectrum/app/images
INSTALLS += sources images

# Deployment

symbian {
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)

    !contains(DEFINES, DISABLE_FFT) {
        # Include FFTReal DLL in the SIS file
        fftreal.sources = ../fftreal.dll
        fftreal.path = !:/sys/bin
        DEPLOYMENT += fftreal
    }
} else {
	DESTDIR = ..$${spectrum_build_dir}
    macx {
        !contains(DEFINES, DISABLE_FFT) {
            # Relocate fftreal.framework into spectrum.app bundle
            framework_dir = ../spectrum.app/Contents/Frameworks
            framework_name = fftreal.framework/Versions/1/fftreal
            QMAKE_POST_LINK = \
                mkdir -p $${framework_dir} &&\
                rm -rf $${framework_dir}/fftreal.framework &&\
                cp -R $${fftreal_dir}/fftreal.framework $${framework_dir} &&\
                install_name_tool -id @executable_path/../Frameworks/$${framework_name} \
                                  $${framework_dir}/$${framework_name} &&\
                install_name_tool -change $${framework_name} \
                                  @executable_path/../Frameworks/$${framework_name} \
                                  ../spectrum.app/Contents/MacOS/spectrum
        }
    } else {
        linux-g++*: {
            # Provide relative path from application to fftreal library
            QMAKE_LFLAGS += -Wl,--rpath=\\\$\$ORIGIN
        }
    }
}


