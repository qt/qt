defineReplace(prependAll) {
    prepend = $$1
    arglist = $$2
    append  = $$3
    for(a,arglist) {
      result += $${prepend}$${a}$${append}
    }
    return ($$result)
}

qtPrepareTool(LUPDATE, lupdate)
LUPDATE += -locations relative -no-ui-lines

###### Qt Libraries

QT_TS        = ar cs da de es fr he hu ja pl pt ru sk sl sv uk zh_CN zh_TW

ts-qt.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                -I../include -I../include/Qt \
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
                                -ts $$prependAll($$QT_SOURCE_TREE/translations/qt_,$$QT_TS,.ts))
ts-qt.depends = sub-tools

###### Designer

ts-designer.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/designer/translations/translations.pro)
ts-designer.depends = sub-tools

###### Linguist

ts-linguist.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/linguist/linguist/linguist.pro)
ts-linguist.depends = sub-tools

###### Assistant

ts-assistant.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/assistant/translations/translations.pro \
                                    && $$LUPDATE \
                                    ../tools/assistant/translations/qt_help.pro)
ts-assistant.depends = sub-tools

###### Qtconfig

ts-qtconfig.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/qtconfig/translations/translations.pro)
ts-qtconfig.depends = sub-tools

###### Qvfp

ts-qvfb.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/qvfb/translations/translations.pro)
ts-qvfb.depends = sub-tools

###### Overall Rules

ts.depends = ts-qt ts-designer ts-linguist ts-assistant ts-qtconfig ts-qvfb

check-ts.commands = (cd $$PWD && perl check-ts.pl)
check-ts.depends = ts

QMAKE_EXTRA_TARGETS += ts-qt ts-designer ts-linguist ts-assistant ts-qtconfig ts-qvfb \
                       ts check-ts
