# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

symbian: {
    load(data_caging_paths)

    SUBDIRS=
    # WARNING: Changing TARGET name will break Symbian SISX upgrade functionality
    # DO NOT TOUCH TARGET VARIABLE IF YOU ARE NOT SURE WHAT YOU ARE DOING
    TARGET = "Qt"
    TARGET.UID3 = 0x2001E61C
    VERSION=$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}

    symbian-abld|symbian-sbsv2 {
        qtresources.sources = $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/s60main.rsc
    } else {
        qtresources.sources = $$QMAKE_LIBDIR_QT/s60main.rsc
        DESTDIR = $$QMAKE_LIBDIR_QT
    }
    qtresources.path = c:$$APP_RESOURCE_DIR

    qtlibraries.sources = \
        $$QMAKE_LIBDIR_QT/QtCore.dll \
        $$QMAKE_LIBDIR_QT/QtXml.dll \
        $$QMAKE_LIBDIR_QT/QtGui.dll \
        $$QMAKE_LIBDIR_QT/QtNetwork.dll \
        $$QMAKE_LIBDIR_QT/QtTest.dll \
        $$QMAKE_LIBDIR_QT/QtSql.dll

    symbian-abld|symbian-sbsv2 {
        pluginLocations = $${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)
    } else {
        pluginLocations = $$QT_BUILD_TREE/plugins/s60
    }

    qts60plugindeployment = \
        "IF package(0x1028315F)" \
        "   \"$$pluginLocations/qts60plugin_5_0.dll\" - \"c:\\sys\\bin\\qts60plugin_5_0.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$$pluginLocations/qts60plugin_3_2.dll\" - \"c:\\sys\\bin\\qts60plugin_3_2.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$$pluginLocations/qts60plugin_3_1.dll\" - \"c:\\sys\\bin\\qts60plugin_3_1.dll\"" \
        "ELSE" \
        "   \"$$pluginLocations/qts60plugin_5_0.dll\" - \"c:\\sys\\bin\\qts60plugin_5_0.dll\"" \
        "ENDIF"
    qtlibraries.pkg_postrules += qts60plugindeployment

    sqlitedeployment = \
        "; Deploy sqlite onto phone that does not have it already" \
        "@\"$$PWD/sqlite3.sis\", (0x2002af5f)"
    qtlibraries.pkg_postrules += sqlitedeployment

    qtlibraries.path = c:/sys/bin

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Nokia, Qt\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Nokia, Qt\"" \
        " "


    qtlibraries.pkg_prerules = vendorinfo
    qtlibraries.pkg_prerules += "; Dependencies of Qt libraries"
    qtlibraries.pkg_prerules += "(0x20013851), 1, 5, 1, {\"PIPS Installer\"}"
    contains(QT_CONFIG, openssl) | contains(QT_CONFIG, openssl-linked) {
        qtlibraries.pkg_prerules += "(0x200110CB), 1, 5, 1, {\"Open C LIBSSL Common\"}"
    }
    contains(CONFIG, stl) {
        qtlibraries.pkg_prerules += "(0x2000F866), 1, 0, 0, {\"Standard C++ Library Common\"}"
    }
    qtlibraries.pkg_prerules += "(0x2002af5f), 0, 5, 0, {\"sqlite3\"}"

    !contains(QT_CONFIG, no-jpeg): imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qjpeg.dll
    !contains(QT_CONFIG, no-gif):  imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qgif.dll
    !contains(QT_CONFIG, no-mng):  imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qmng.dll
    !contains(QT_CONFIG, no-tiff): imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qtiff.dll
    !contains(QT_CONFIG, no-ico):  imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qico.dll
    imageformats_plugins.path = c:$$QT_PLUGINS_BASE_DIR/imageformats

    codecs_plugins.sources = $$QT_BUILD_TREE/plugins/codecs/qcncodecs.dll $$QT_BUILD_TREE/plugins/codecs/qjpcodecs.dll $$QT_BUILD_TREE/plugins/codecs/qtwcodecs.dll $$QT_BUILD_TREE/plugins/codecs/qkrcodecs.dll
    codecs_plugins.path = c:$$QT_PLUGINS_BASE_DIR/codecs

    contains(QT_CONFIG, phonon-backend) {
        phonon_backend_plugins.sources += $$QMAKE_LIBDIR_QT/phonon_mmf.dll

        phonon_backend_plugins.path = c:$$QT_PLUGINS_BASE_DIR/phonon_backend
        DEPLOYMENT += phonon_backend_plugins
    }

    # Support backup & restore for Qt libraries
    qtbackup.sources = backup_registration.xml
    qtbackup.path = c:/private/10202D56/import/packages/$$replace(TARGET.UID3, 0x,)

    bearer_plugins.path = c:$$QT_PLUGINS_BASE_DIR/bearer
    bearer_plugins.sources += $$QT_BUILD_TREE/plugins/bearer/qsymbianbearer.dll

    DEPLOYMENT += qtresources \
                  qtlibraries \
                  qtbackup \
                  imageformats_plugins \
                  codecs_plugins \
                  graphicssystems_plugins \
                  bearer_plugins

    contains(QT_CONFIG, svg): {
       qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtSvg.dll
       imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qsvg.dll
       iconengines_plugins.sources = $$QT_BUILD_TREE/plugins/iconengines/qsvgicon.dll
       iconengines_plugins.path = c:$$QT_PLUGINS_BASE_DIR/iconengines
       DEPLOYMENT += iconengines_plugins
    }

    contains(QT_CONFIG, phonon): {
       qtlibraries.sources += $$QMAKE_LIBDIR_QT/phonon.dll
    }

    contains(QT_CONFIG, script): {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtScript.dll
    }

    contains(QT_CONFIG, xmlpatterns): {
       qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtXmlPatterns.dll
    }

    contains(QT_CONFIG, declarative): {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtDeclarative.dll
    }

    graphicssystems_plugins.path = c:$$QT_PLUGINS_BASE_DIR/graphicssystems
    contains(QT_CONFIG, openvg) {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtOpenVG.dll
        graphicssystems_plugins.sources += $$QT_BUILD_TREE/plugins/graphicssystems/qvggraphicssystem.dll
    }

    contains(QT_CONFIG, multimedia) {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtMultimedia.dll
    }

    BLD_INF_RULES.prj_exports += "qt.iby $$CORE_MW_LAYER_IBY_EXPORT_PATH(qt.iby)"
    BLD_INF_RULES.prj_exports += "qtdemoapps.iby $$CUSTOMER_VARIANT_APP_LAYER_IBY_EXPORT_PATH(qtdemoapps.iby)"
}
