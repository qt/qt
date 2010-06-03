# Use subdirs template to suppress generation of unnecessary files
TEMPLATE = subdirs

symbian: {
    load(data_caging_paths)

    SUBDIRS=
    # WARNING: Changing TARGET name will break Symbian SISX upgrade functionality
    # DO NOT TOUCH TARGET VARIABLE IF YOU ARE NOT SURE WHAT YOU ARE DOING
    TARGET = "Qt$${QT_LIBINFIX}"

    isEmpty(QT_LIBINFIX) {
        TARGET.UID3 = 0x2001E61C

        # sqlite3 is expected to be already found on phone if infixed configuration is built.
        BLD_INF_RULES.prj_exports += \
            "sqlite3.sis $${EPOCROOT}epoc32/data/qt/sis/sqlite3.sis" \
            "sqlite3_selfsigned.sis $${EPOCROOT}epoc32/data/qt/sis/sqlite3_selfsigned.sis"
        symbian-abld|symbian-sbsv2 {
            sqlitedeployment = \
                "; Deploy sqlite onto phone that does not have it already" \
                "@\"$${EPOCROOT}epoc32/data/qt/sis/sqlite3.sis\", (0x2002af5f)"
        } else {
            sqlitedeployment = \
                "; Deploy sqlite onto phone that does not have it already" \
                "@\"$${PWD}/sqlite3.sis\", (0x2002af5f)"
        }
        qtlibraries.pkg_postrules += sqlitedeployment
    } else {
        # Always use experimental UID for infixed configuration to avoid UID clash
        TARGET.UID3 = 0xE001E61C
    }
    VERSION=$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}

    symbian-abld|symbian-sbsv2 {
        qtresources.sources = $${EPOCROOT}$$HW_ZDIR$$APP_RESOURCE_DIR/s60main$${QT_LIBINFIX}.rsc
    } else {
        qtresources.sources = $$QMAKE_LIBDIR_QT/s60main$${QT_LIBINFIX}.rsc
        DESTDIR = $$QMAKE_LIBDIR_QT
    }
    qtresources.path = c:$$APP_RESOURCE_DIR
    DEPLOYMENT += qtresources

    qtlibraries.sources = \
        $$QMAKE_LIBDIR_QT/QtCore$${QT_LIBINFIX}.dll \
        $$QMAKE_LIBDIR_QT/QtXml$${QT_LIBINFIX}.dll \
        $$QMAKE_LIBDIR_QT/QtGui$${QT_LIBINFIX}.dll \
        $$QMAKE_LIBDIR_QT/QtNetwork$${QT_LIBINFIX}.dll \
        $$QMAKE_LIBDIR_QT/QtTest$${QT_LIBINFIX}.dll \
        $$QMAKE_LIBDIR_QT/QtSql$${QT_LIBINFIX}.dll

    symbian-abld|symbian-sbsv2 {
        pluginLocations = $${EPOCROOT}epoc32/release/$(PLATFORM)/$(TARGET)
    } else {
        pluginLocations = $$QT_BUILD_TREE/plugins/s60
    }

    qts60plugindeployment = \
        "IF package(0x1028315F)" \
        "   \"$$pluginLocations/qts60plugin_5_0$${QT_LIBINFIX}.dll\" - \"c:\\sys\\bin\\qts60plugin_5_0$${QT_LIBINFIX}.dll\"" \
        "ELSEIF package(0x102752AE)" \
        "   \"$$pluginLocations/qts60plugin_3_2$${QT_LIBINFIX}.dll\" - \"c:\\sys\\bin\\qts60plugin_3_2$${QT_LIBINFIX}.dll\"" \
        "ELSEIF package(0x102032BE)" \
        "   \"$$pluginLocations/qts60plugin_3_1$${QT_LIBINFIX}.dll\" - \"c:\\sys\\bin\\qts60plugin_3_1$${QT_LIBINFIX}.dll\"" \
        "ELSE" \
        "   \"$$pluginLocations/qts60plugin_5_0$${QT_LIBINFIX}.dll\" - \"c:\\sys\\bin\\qts60plugin_5_0$${QT_LIBINFIX}.dll\"" \
        "ENDIF"
    qtlibraries.pkg_postrules += qts60plugindeployment


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

    !contains(QT_CONFIG, no-jpeg): imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qjpeg$${QT_LIBINFIX}.dll
    !contains(QT_CONFIG, no-gif):  imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qgif$${QT_LIBINFIX}.dll
    !contains(QT_CONFIG, no-mng):  imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qmng$${QT_LIBINFIX}.dll
    !contains(QT_CONFIG, no-tiff): imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qtiff$${QT_LIBINFIX}.dll
    !contains(QT_CONFIG, no-ico):  imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qico$${QT_LIBINFIX}.dll
    imageformats_plugins.path = c:$$QT_PLUGINS_BASE_DIR/imageformats

    codecs_plugins.sources = $$QT_BUILD_TREE/plugins/codecs/qcncodecs$${QT_LIBINFIX}.dll $$QT_BUILD_TREE/plugins/codecs/qjpcodecs$${QT_LIBINFIX}.dll $$QT_BUILD_TREE/plugins/codecs/qtwcodecs$${QT_LIBINFIX}.dll $$QT_BUILD_TREE/plugins/codecs/qkrcodecs$${QT_LIBINFIX}.dll
    codecs_plugins.path = c:$$QT_PLUGINS_BASE_DIR/codecs

    contains(QT_CONFIG, phonon-backend) {
        phonon_backend_plugins.sources += $$QMAKE_LIBDIR_QT/phonon_mmf$${QT_LIBINFIX}.dll

        phonon_backend_plugins.path = c:$$QT_PLUGINS_BASE_DIR/phonon_backend
        DEPLOYMENT += phonon_backend_plugins
    }

    # Support backup & restore for Qt libraries
    qtbackup.sources = backup_registration.xml
    qtbackup.path = c:/private/10202D56/import/packages/$$replace(TARGET.UID3, 0x,)

    bearer_plugins.path = c:$$QT_PLUGINS_BASE_DIR/bearer
    bearer_plugins.sources += $$QT_BUILD_TREE/plugins/bearer/qsymbianbearer$${QT_LIBINFIX}.dll

    DEPLOYMENT += qtlibraries \
                  qtbackup \
                  imageformats_plugins \
                  codecs_plugins \
                  graphicssystems_plugins \
                  bearer_plugins

    contains(QT_CONFIG, svg): {
       qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtSvg$${QT_LIBINFIX}.dll
       imageformats_plugins.sources += $$QT_BUILD_TREE/plugins/imageformats/qsvg$${QT_LIBINFIX}.dll
       iconengines_plugins.sources = $$QT_BUILD_TREE/plugins/iconengines/qsvgicon$${QT_LIBINFIX}.dll
       iconengines_plugins.path = c:$$QT_PLUGINS_BASE_DIR/iconengines
       DEPLOYMENT += iconengines_plugins
    }

    contains(QT_CONFIG, phonon): {
       qtlibraries.sources += $$QMAKE_LIBDIR_QT/phonon$${QT_LIBINFIX}.dll
    }

    contains(QT_CONFIG, script): {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtScript$${QT_LIBINFIX}.dll
    }

    contains(QT_CONFIG, xmlpatterns): {
       qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtXmlPatterns$${QT_LIBINFIX}.dll
    }

    contains(QT_CONFIG, declarative): {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtDeclarative$${QT_LIBINFIX}.dll
    }

    graphicssystems_plugins.path = c:$$QT_PLUGINS_BASE_DIR/graphicssystems
    contains(QT_CONFIG, openvg) {
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtOpenVG$${QT_LIBINFIX}.dll
        graphicssystems_plugins.sources += $$QT_BUILD_TREE/plugins/graphicssystems/qvggraphicssystem$${QT_LIBINFIX}.dll
    }

    contains(QT_CONFIG, multimedia){
        qtlibraries.sources += $$QMAKE_LIBDIR_QT/QtMultimedia$${QT_LIBINFIX}.dll
    }

    BLD_INF_RULES.prj_exports += "qt.iby $$CORE_MW_LAYER_IBY_EXPORT_PATH(qt.iby)"
    BLD_INF_RULES.prj_exports += "qtdemoapps.iby $$CUSTOMER_VARIANT_APP_LAYER_IBY_EXPORT_PATH(qtdemoapps.iby)"
}
