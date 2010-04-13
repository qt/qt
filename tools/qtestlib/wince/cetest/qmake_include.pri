#qmake source files needed for cetest
HEADERS += \
           $$QT_SOURCE_TREE/qmake/option.h
SOURCES += \
           $$QT_SOURCE_TREE/qmake/option.cpp \
           $$QT_SOURCE_TREE/qmake/project.cpp \
           $$QT_SOURCE_TREE/qmake/property.cpp \
           $$QT_SOURCE_TREE/qmake/generators/metamakefile.cpp \
           $$QT_SOURCE_TREE/qmake/generators/symbian/initprojectdeploy_symbian.cpp \
           $$QT_SOURCE_TREE/tools/shared/symbian/epocroot.cpp \
           $$QT_SOURCE_TREE/tools/shared/windows/registry.cpp

DEFINES += QT_QMAKE_PARSER_ONLY
