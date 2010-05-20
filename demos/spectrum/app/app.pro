include(../spectrum.pri)

TEMPLATE = app

TARGET = spectrum
unix: !macx: !symbian: TARGET = spectrum.bin

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
    TARGET.CAPABILITY += UserEnvironment

    # Provide unique ID for the generated binary, required by Symbian OS
    TARGET.UID3 = 0xA000E3FA
}


# Dynamic linkage against FFTReal DLL
!contains(DEFINES, DISABLE_FFT) {
    symbian {
        # Must explicitly add the .dll suffix to ensure dynamic linkage
        LIBS += -lfftreal.dll
    } else {
        macx {
            # Link to fftreal framework
            LIBS += -F$${fftreal_dir}
            LIBS += -framework fftreal
        } else {
            # Link to dynamic library which is written to ../bin
            LIBS += -L../bin
            LIBS += -lfftreal
        }
    }
}


# Deployment

symbian {
    include($$QT_SOURCE_TREE/demos/symbianpkgrules.pri)

    !contains(DEFINES, DISABLE_FFT) {
        # Include FFTReal DLL in the SIS file
        fftreal.sources = $${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)/fftreal.dll
        fftreal.path = !:/sys/bin
        DEPLOYMENT += fftreal
    }
} else {
    macx {
        # Specify directory in which to create spectrum.app bundle
        DESTDIR = ..

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
        # Specify directory in which to create spectrum application
        DESTDIR = ../bin

        unix: !symbian {
            # On unices other than Mac OSX, we copy a shell script into the bin directory.
            # This script takes care of correctly setting the LD_LIBRARY_PATH so that
            # the dynamic library can be located.
            copy_launch_script.target = copy_launch_script
            copy_launch_script.commands = \
                install -m 0555 spectrum.sh ../bin/spectrum
            QMAKE_EXTRA_TARGETS += copy_launch_script
            POST_TARGETDEPS += copy_launch_script
        }
    }
}


