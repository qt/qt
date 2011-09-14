TRANSLATIONS = $$files(*.ts)

qtPrepareTool(LRELEASE, lrelease)
qtPrepareTool(LCONVERT, lconvert)
qtPrepareTool(LUPDATE, lupdate)
LUPDATE += -locations relative -no-ui-lines

TS_TARGETS =

# meta target name, target name, lupdate base options, files
defineTest(addTsTarget) {
    cv = $${2}.commands
    $$cv = cd $$QT_SOURCE_TREE/src && $$LUPDATE $$3 -ts $$4
    export($$cv)
    dv = $${1}.depends
    $$dv += $$2
    export($$dv)
    TS_TARGETS += $$1 $$2
    export(TS_TARGETS)
}

# target basename, lupdate base options
defineTest(addTsTargets) {
    files = $$files($$PWD/$${1}_??.ts) $$files($$PWD/$${1}_??_??.ts)
    for(file, files) {
        lang = $$replace(file, .*_((.._)?..)\\.ts$, \\1)
        addTsTarget(ts-$$lang, ts-$$1-$$lang, $$2, $$file)
    }
    addTsTarget(ts-untranslated, ts-$$1-untranslated, $$2, $$PWD/$${1}_untranslated.ts)
    addTsTarget(ts-all, ts-$$1-all, $$2, $$PWD/$${1}_untranslated.ts $$files)
}

addTsTargets(qt, -I../include -I../include/Qt \
    3rdparty/phonon \
    3rdparty/webkit \
    activeqt \
    corelib \
    declarative \
    gui \
    multimedia \
    network \
    opengl \
    plugins \
    qt3support \
    script \
    scripttools \
    sql \
    svg \
    xml \
    xmlpatterns \
)
addTsTargets(designer, ../tools/designer/designer.pro)
addTsTargets(linguist, ../tools/linguist/linguist.pro)
addTsTargets(assistant, ../tools/assistant/tools/tools.pro)
addTsTargets(qt_help, ../tools/assistant/lib/lib.pro)
addTsTargets(qtconfig, ../tools/qtconfig/qtconfig.pro)
addTsTargets(qvfb, ../tools/qvfb/qvfb.pro)
addTsTargets(qmlviewer, ../tools/qml/qml.pro)

check-ts.commands = (cd $$PWD && perl check-ts.pl)
check-ts.depends = ts-all

isEqual(QMAKE_DIR_SEP, /) {
    commit-ts.commands = \
        cd $$PWD/..; \
        for f in `git diff-files --name-only translations/*_??.ts`; do \
            $$LCONVERT -locations none -i \$\$f -o \$\$f; \
        done; \
        git add translations/*_??.ts && git commit
} else {
    wd = $$replace(PWD, /, \\)\\..
    commit-ts.commands = \
        cd $$wd && \
        for /f usebackq %%f in (`git diff-files --name-only translations/*_??.ts`) do \
            $$LCONVERT -locations none -i %%f -o %%f $$escape_expand(\\n\\t) \
        cd $$wd && git add translations/*_??.ts && git commit
}

ts.commands = \
    @echo \"The \'ts\' target has been removed in favor of more fine-grained targets.\" && \
    echo \"Use \'ts-<target>-<lang>\' or \'ts-<lang>\' instead. To add a language,\" && \
    echo \"use \'untranslated\' for <lang>, rename the files and re-run \'qmake\'.\"

QMAKE_EXTRA_TARGETS += $$unique(TS_TARGETS) ts commit-ts check-ts

TEMPLATE = app
TARGET = qm_phony_target
CONFIG -= qt separate_debug_info gdb_dwarf_index sis_targets
CONFIG += no_icon
QT =
LIBS =

contains(TEMPLATE_PREFIX, vc):vcproj = 1

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
