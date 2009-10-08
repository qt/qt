include(../src/components/buddyeditor/buddyeditor.pri)
include(../src/components/component.pri)
include(../src/components/formeditor/formeditor.pri)
include(../src/components/objectinspector/objectinspector.pri)
include(../src/components/propertyeditor/propertyeditor.pri)
include(../src/components/signalsloteditor/signalsloteditor.pri)
include(../src/components/tabordereditor/tabordereditor.pri)
include(../src/components/taskmenu/taskmenu.pri)
include(../src/components/widgetbox/widgetbox.pri)
include(../src/lib/extension/extension.pri)
include(../src/lib/sdk/sdk.pri)
include(../src/lib/shared/shared.pri)
include(../src/lib/uilib/uilib.pri)
include(../../shared/qttoolbardialog/qttoolbardialog.pri)
include(../../shared/qtpropertybrowser/qtpropertybrowser.pri)
include(../../shared/qtgradienteditor/qtgradienteditor.pri)

# Include ActiveQt plugin
SOURCES += ../src/plugins/activeqt/qaxwidgetextrainfo.cpp \
           ../src/plugins/activeqt/qaxwidgetplugin.cpp \
           ../src/plugins/activeqt/qaxwidgetpropertysheet.cpp \
           ../src/plugins/activeqt/qaxwidgettaskmenu.cpp \
           ../src/plugins/activeqt/qdesigneraxwidget.cpp

HEADERS += ../src/plugins/activeqt/qaxwidgetextrainfo.h \
           ../src/plugins/activeqt/qaxwidgetplugin.h \
           ../src/plugins/activeqt/qaxwidgetpropertysheet.h \
           ../src/plugins/activeqt/qaxwidgettaskmenu.h \
           ../src/plugins/activeqt/qdesigneraxwidget.h \
           ../../../src/activeqt/shared/qaxtypes.h


# Include Qt3Support plugin

SOURCES += ../src/plugins/widgets/qt3supportwidgets.cpp
HEADERS += ../src/plugins/widgets/q3iconview/q3iconview_extrainfo.h \
           ../src/plugins/widgets/q3iconview/q3iconview_plugin.h \
           ../src/plugins/widgets/q3listview/q3listview_extrainfo.h \
           ../src/plugins/widgets/q3listview/q3listview_plugin.h \
           ../src/plugins/widgets/q3mainwindow/q3mainwindow_container.h \
           ../src/plugins/widgets/q3mainwindow/q3mainwindow_plugin.h \
           ../src/plugins/widgets/q3toolbar/q3toolbar_extrainfo.h \
           ../src/plugins/widgets/q3toolbar/q3toolbar_plugin.h \
           ../src/plugins/widgets/q3widgetstack/q3widgetstack_container.h \
           ../src/plugins/widgets/q3widgetstack/q3widgetstack_plugin.h \
           ../src/plugins/widgets/q3widgetstack/qdesigner_q3widgetstack_p.h \
           ../src/plugins/widgets/q3wizard/q3wizard_container.h \
           ../src/plugins/widgets/q3wizard/q3wizard_plugin.h \
           ../src/plugins/widgets/q3listbox/q3listbox_extrainfo.h \
           ../src/plugins/widgets/q3listbox/q3listbox_plugin.h \
           ../src/plugins/widgets/q3table/q3table_extrainfo.h \
           ../src/plugins/widgets/q3table/q3table_plugin.h \
           ../src/plugins/widgets/q3textedit/q3textedit_extrainfo.h \
           ../src/plugins/widgets/q3textedit/q3textedit_plugin.h \
           ../src/plugins/widgets/q3widgets/q3widget_plugins.h

SOURCES += ../src/plugins/widgets/q3iconview/q3iconview_extrainfo.cpp \
           ../src/plugins/widgets/q3iconview/q3iconview_plugin.cpp \
           ../src/plugins/widgets/q3listview/q3listview_extrainfo.cpp \
           ../src/plugins/widgets/q3listview/q3listview_plugin.cpp \
           ../src/plugins/widgets/q3mainwindow/q3mainwindow_container.cpp \
           ../src/plugins/widgets/q3mainwindow/q3mainwindow_plugin.cpp \
           ../src/plugins/widgets/q3toolbar/q3toolbar_extrainfo.cpp \
           ../src/plugins/widgets/q3toolbar/q3toolbar_plugin.cpp \
           ../src/plugins/widgets/q3widgetstack/q3widgetstack_container.cpp \
           ../src/plugins/widgets/q3widgetstack/q3widgetstack_plugin.cpp \
           ../src/plugins/widgets/q3widgetstack/qdesigner_q3widgetstack.cpp \
           ../src/plugins/widgets/q3wizard/q3wizard_container.cpp \
           ../src/plugins/widgets/q3wizard/q3wizard_plugin.cpp \
           ../src/plugins/widgets/q3listbox/q3listbox_extrainfo.cpp \
           ../src/plugins/widgets/q3listbox/q3listbox_plugin.cpp \
           ../src/plugins/widgets/q3table/q3table_extrainfo.cpp \
           ../src/plugins/widgets/q3table/q3table_plugin.cpp \
           ../src/plugins/widgets/q3textedit/q3textedit_extrainfo.cpp \
           ../src/plugins/widgets/q3textedit/q3textedit_plugin.cpp \
           ../src/plugins/widgets/q3widgets/q3widget_plugins.cpp

# Include those manually as they do not contain any directory specification
APP_DIR=../src/designer
SOURCES += $$APP_DIR/appfontdialog.cpp \
           $$APP_DIR/assistantclient.cpp \
           $$APP_DIR/main.cpp \
           $$APP_DIR/mainwindow.cpp \
           $$APP_DIR/newform.cpp \
           $$APP_DIR/preferencesdialog.cpp \
           $$APP_DIR/qdesigner_actions.cpp \
           $$APP_DIR/qdesigner_appearanceoptions.cpp \
           $$APP_DIR/qdesigner.cpp \
           $$APP_DIR/qdesigner_formwindow.cpp \
           $$APP_DIR/qdesigner_server.cpp \
           $$APP_DIR/qdesigner_settings.cpp \
           $$APP_DIR/qdesigner_toolwindow.cpp \
           $$APP_DIR/qdesigner_workbench.cpp \
           $$APP_DIR/saveformastemplate.cpp \
           $$APP_DIR/versiondialog.cpp

HEADERS+=  $$APP_DIR/appfontdialog.h \
           $$APP_DIR/assistantclient.h \
           $$APP_DIR/designer_enums.h \
           $$APP_DIR/mainwindow.h \
           $$APP_DIR/newform.h \
           $$APP_DIR/preferencesdialog.h \
           $$APP_DIR/qdesigner_actions.h \
           $$APP_DIR/qdesigner_appearanceoptions.h \
           $$APP_DIR/qdesigner_formwindow.h \
           $$APP_DIR/qdesigner.h \
           $$APP_DIR/qdesigner_pch.h \
           $$APP_DIR/qdesigner_server.h \
           $$APP_DIR/qdesigner_settings.h \
           $$APP_DIR/qdesigner_toolwindow.h \
           $$APP_DIR/qdesigner_workbench.h \
           $$APP_DIR/saveformastemplate.h \
           $$APP_DIR/versiondialog.h

FORMS +=   $$APP_DIR/preferencesdialog.ui \
           $$APP_DIR/qdesigner_appearanceoptions.ui \
           $$APP_DIR/saveformastemplate.ui

# Shared solutions
SOURCES += ../../shared/fontpanel/fontpanel.cpp \
           ../../shared/deviceskin/deviceskin.cpp \
           ../../shared/findwidget/abstractfindwidget.cpp \
           ../../shared/findwidget/itemviewfindwidget.cpp \
           ../../shared/findwidget/texteditfindwidget.cpp \

HEADERS += ../../shared/findwidget/abstractfindwidget.h \
           ../../shared/findwidget/itemviewfindwidget.h \
           ../../shared/findwidget/texteditfindwidget.h

TR_DIR = $$PWD/../../../translations
TRANSLATIONS = \
    $$TR_DIR/designer_de.ts \
    $$TR_DIR/designer_ja.ts \
    $$TR_DIR/designer_pl.ts \
    $$TR_DIR/designer_ru.ts \
    $$TR_DIR/designer_sl.ts \
    $$TR_DIR/designer_zh_CN.ts \
    $$TR_DIR/designer_zh_TW.ts
