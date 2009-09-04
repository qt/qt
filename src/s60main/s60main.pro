# Additional Qt project file for qtmain lib on Symbian
TEMPLATE = lib
TARGET	 = qtmain
DESTDIR	 = $$QMAKE_LIBDIR_QT
QT       =

CONFIG	+= staticlib warn_on
CONFIG	-= qt shared

symbian {
    # Note: UID only needed for ensuring that no filename generation conflicts occur
    TARGET.UID3 = 0x2001E61F
    CONFIG      +=  png zlib
    CONFIG	-=  jpeg
    INCLUDEPATH	+=  tmp $$QMAKE_INCDIR_QT/QtCore $$MW_LAYER_SYSTEMINCLUDE
    SOURCES	 =  qts60main.cpp \
                    qts60main_mcrt0.cpp

    # This block serves the minimalistic resource file for S60 3.1 platforms.
    # Note there is no way to ifdef S60 version in mmp file, that is why the resource
    # file is always compiled for WINSCW
    minimalAppResource31 = \
        "START RESOURCE s60main.rss" \
        "HEADER" \
        "TARGETPATH resource\apps" \
        "END"
    MMP_RULES += minimalAppResource31

    # s60main needs to be built in ARM mode for GCCE to work.
    MMP_RULES+="ALWAYS_BUILD_AS_ARM"

    # staticlib should not have any lib depencies in s60
    # This seems not to work, some hard coded libs are still added as dependency
    LIBS =
} else {
    error("$$_FILE_ is intended only for Symbian!")
}

symbian-abld: {
    # abld build commands generated resources after the static library is built, and
    # we have dependency to resource from static lib -> resources need to be generated
    # explicitly before library
    rsgFix2.commands = "-$(DEL_FILE) $(EPOCROOT)Epoc32\Data\z\resource\apps\s60main.rsc >NUL 2>&1"
    rsgFix.commands = "-$(ABLD) resource $(PLATFORM) $(CFG) 2>NUL"
    QMAKE_EXTRA_TARGETS += rsgFix rsgFix2
    PRE_TARGETDEPS += rsgFix rsgFix2
}

include(../qbase.pri)
