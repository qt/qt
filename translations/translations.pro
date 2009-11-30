TRANSLATIONS = $$files(*.ts)

LRELEASE = $$QT_BUILD_TREE/bin/lrelease
LRELEASE ~= s,/,$$QMAKE_DIR_SEP,

contains(TEMPLATE_PREFIX, vc):vcproj = 1

TEMPLATE = app
TARGET = qm_phony_target
CONFIG -= qt separate_debug_info
QT =
LIBS =

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = @echo lrelease ${QMAKE_FILE_IN}; $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm

isEmpty(vcproj) {
    QMAKE_LINK = @: IGNORE THIS LINE
    OBJECTS_DIR =
    win32:CONFIG -= embed_manifest_exe
} else {
    CONFIG += console
    PHONY_DEPS = .
    phony_src.input = PHONY_DEPS
    phony_src.output = phony.c
    phony_src.variable_out = GENERATED_SOURCES
    phony_src.commands = echo int main() { return 0; } > phony.c
    phony_src.name = CREATE phony.c
    phony_src.CONFIG += combine
    QMAKE_EXTRA_COMPILERS += phony_src
}

translations.path = $$[QT_INSTALL_TRANSLATIONS]
translations.files = $$TRANSLATIONS
translations.files ~= s,\\.ts$,.qm,g
translations.CONFIG += no_check_exist
INSTALLS += translations
