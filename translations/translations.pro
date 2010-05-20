TRANSLATIONS = $$files(*.ts)

qtPrepareTool(LRELEASE, lrelease)

contains(TEMPLATE_PREFIX, vc):vcproj = 1

TEMPLATE = app
TARGET = qm_phony_target
CONFIG -= qt separate_debug_info sis_targets
CONFIG += no_icon
QT =
LIBS =

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_BASE}.qm
isEmpty(vcproj):updateqm.variable_out = PRE_TARGETDEPS
updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
silent:updateqm.commands = @echo lrelease ${QMAKE_FILE_IN} && $$updateqm.commands
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
translations.files ~= s,^,$$OUT_PWD/,g
translations.CONFIG += no_check_exist
INSTALLS += translations

# Make dummy "sis" target to keep recursive "make sis" working.
sis_target.target = sis
sis_target.commands =
QMAKE_EXTRA_TARGETS += sis_target
