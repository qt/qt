/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "symbiancommon.h"
#include <qdebug.h>

// Included from tools/shared
#include <symbian/epocroot.h>

#define RESOURCE_DIRECTORY_RESOURCE "\\\\resource\\\\apps\\\\"

#define RSS_RULES "RSS_RULES"
#define RSS_RULES_BASE "RSS_RULES."
#define RSS_TAG_NBROFICONS "number_of_icons"
#define RSS_TAG_ICONFILE "icon_file"
#define RSS_TAG_HEADER "header"
#define RSS_TAG_SERVICE_LIST "service_list"
#define RSS_TAG_FILE_OWNERSHIP_LIST "file_ownership_list"
#define RSS_TAG_DATATYPE_LIST "datatype_list"
#define RSS_TAG_FOOTER "footer"
#define RSS_TAG_DEFAULT "default_rules" // Same as just giving rules without tag

#define PLUGIN_COMMON_DEF_FILE_ACTUAL "plugin_commonu.def"

#define MANUFACTURER_NOTE_FILE "manufacturer_note.txt"
#define DEFAULT_MANUFACTURER_NOTE \
    "The package is not supported for devices from this manufacturer. Please try the selfsigned " \
    "version of the package instead."

SymbianCommonGenerator::SymbianCommonGenerator(MakefileGenerator *generator)
    : generator(generator)
{
}

void SymbianCommonGenerator::init()
{
    QMakeProject *project = generator->project;
    fixedTarget = project->first("QMAKE_ORIG_TARGET");
    if (fixedTarget.isEmpty())
        fixedTarget = project->first("TARGET");
    fixedTarget = generator->unescapeFilePath(fixedTarget);
    fixedTarget = removePathSeparators(fixedTarget);
    if (project->first("MAKEFILE_GENERATOR") == "SYMBIAN_ABLD"
        || project->first("MAKEFILE_GENERATOR") == "SYMBIAN_SBSV2")
        removeEpocSpecialCharacters(fixedTarget);
    else
        removeSpecialCharacters(fixedTarget);

    // This should not be empty since the mkspecs are supposed to set it if missing.
    uid3 = project->first("TARGET.UID3").trimmed();

    if ((project->values("TEMPLATE")).contains("app"))
        targetType = TypeExe;
    else if ((project->values("TEMPLATE")).contains("lib")) {
        // Check CONFIG to see if we are to build staticlib or dll
        if (project->isActiveConfig("staticlib") || project->isActiveConfig("static"))
            targetType = TypeLib;
        else if (project->isActiveConfig("plugin"))
            targetType = TypePlugin;
        else
            targetType = TypeDll;
    } else {
        targetType = TypeSubdirs;
    }

    // UID is valid as either hex or decimal, so just convert it to number and back to hex
    // to get proper string for private dir
    bool conversionOk = false;
    uint uidNum = uid3.toUInt(&conversionOk, 0);

    if (!conversionOk) {
        fprintf(stderr, "Error: Invalid UID \"%s\".\n", uid3.toUtf8().constData());
    } else {
        privateDirUid.setNum(uidNum, 16);
        while (privateDirUid.length() < 8)
            privateDirUid.insert(0, QLatin1Char('0'));
    }
}

bool SymbianCommonGenerator::containsStartWithItem(const QChar &c, const QStringList& src)
{
    bool result = false;
    foreach(QString str, src) {
        if (str.startsWith(c)) {
            result =  true;
            break;
        }
    }
    return result;
}

void SymbianCommonGenerator::removeSpecialCharacters(QString& str)
{
    // When modifying this method check also application_icon.prf
    str.replace(QString("/"), QString("_"));
    str.replace(QString("\\"), QString("_"));
    str.replace(QString(" "), QString("_"));
}

void SymbianCommonGenerator::removeEpocSpecialCharacters(QString& str)
{
    // When modifying this method check also application_icon.prf
    str.replace(QString("-"), QString("_"));
    str.replace(QString(":"), QString("_"));
    str.replace(QString("."), QString("_"));
    removeSpecialCharacters(str);
}

QString romPath(const QString& path)
{
    if(path.length() > 2 && path[1] == ':')
        return QLatin1String("z:") + path.mid(2);
    return QLatin1String("z:") + path;
}

void SymbianCommonGenerator::generatePkgFile(const QString &iconFile, bool epocBuild)
{
    QMakeProject *project = generator->project;
    QString pkgTarget = project->first("QMAKE_ORIG_TARGET");
    if (pkgTarget.isEmpty())
        pkgTarget = project->first("TARGET");
    pkgTarget = generator->unescapeFilePath(pkgTarget);
    pkgTarget = removePathSeparators(pkgTarget);
    QString pkgFilename = Option::output_dir + QLatin1Char('/') +
                          QString("%1_template.pkg").arg(pkgTarget);

    QFile pkgFile(pkgFilename);
    if (!pkgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        PRINT_FILE_CREATE_ERROR(pkgFilename);
        return;
    }

    QString stubPkgFileName = Option::output_dir + QLatin1Char('/') +
                          QString("%1_stub.pkg").arg(pkgTarget);

    QFile stubPkgFile(stubPkgFileName);
    if (!stubPkgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        PRINT_FILE_CREATE_ERROR(stubPkgFileName);
        return;
    }

    generatedFiles << pkgFile.fileName();
    QTextStream t(&pkgFile);
    generatedFiles << stubPkgFile.fileName();
    QTextStream ts(&stubPkgFile);

    QString installerSisHeader = project->values("DEPLOYMENT.installer_header").join("\n");
    if (installerSisHeader.isEmpty())
        installerSisHeader = "0xA000D7CE"; // Use default self-signable UID if not defined

    QString wrapperStreamBuffer;
    QTextStream tw(&wrapperStreamBuffer);

    QString dateStr = QDateTime::currentDateTime().toString(Qt::ISODate);

    // Header info
    QString wrapperPkgFilename = Option::output_dir + QLatin1Char('/') + QString("%1_installer.%2")
                                 .arg(pkgTarget).arg("pkg");

    QString headerComment = "; %1 generated by qmake at %2\n"
        "; This file is generated by qmake and should not be modified by the user\n"
        ";\n\n";
    t << headerComment.arg(pkgFilename).arg(dateStr);
    tw << headerComment.arg(wrapperPkgFilename).arg(dateStr);
    ts << headerComment.arg(stubPkgFileName).arg(dateStr);

    // Construct QStringList from pkg_prerules since we need search it before printed to file
    // Note: Though there can't be more than one language or header line, use stringlists
    // in case user wants comments to go with the rules.
    QStringList rawPkgPreRules;
    QStringList languageRules;
    QStringList headerRules;
    foreach(QString deploymentItem, project->values("DEPLOYMENT")) {
        foreach(QString pkgrulesItem, project->values(deploymentItem + ".pkg_prerules")) {
            QStringList pkgrulesValue = project->values(pkgrulesItem);
            // If there is no stringlist defined for a rule, use rule name directly
            // This is convenience for defining single line mmp statements
            if (pkgrulesValue.isEmpty()) {
                if (pkgrulesItem.startsWith("&"))
                    languageRules << pkgrulesItem;
                else if (pkgrulesItem.startsWith("#"))
                    headerRules << pkgrulesItem;
                else
                    rawPkgPreRules << pkgrulesItem;
            } else {
                if (containsStartWithItem('&', pkgrulesValue)) {
                    foreach(QString pkgrule, pkgrulesValue) {
                        languageRules << pkgrule;
                    }
                } else if (containsStartWithItem('#', pkgrulesValue)) {
                    foreach(QString pkgrule, pkgrulesValue) {
                        headerRules << pkgrule;
                    }
                } else {
                    foreach(QString pkgrule, pkgrulesValue) {
                        rawPkgPreRules << pkgrule;
                    }
                }
            }
        }
    }

    // Apply some defaults if specific data does not exist in PKG pre-rules

    if (languageRules.isEmpty()) {
        // language, (*** hardcoded to english atm, should be parsed from TRANSLATIONS)
        languageRules << "; Language\n&EN\n\n";
    } else if (headerRules.isEmpty()) {
        // In case user defines langs, he must take care also about SIS header
        fprintf(stderr, "Warning: If language is defined with DEPLOYMENT pkg_prerules, also the SIS header must be defined\n");
    }

    t << languageRules.join("\n") << endl;
    tw << languageRules.join("\n") << endl;
    ts << languageRules.join("\n") << endl;

    // name of application, UID and version
    QString applicationVersion = project->first("VERSION").isEmpty() ? "1,0,0" : project->first("VERSION").replace('.', ',');
    QString sisHeader = "; SIS header: name, uid, version\n#{\"%1\"},(%2),%3\n\n";
    QString visualTarget = generator->escapeFilePath(project->first("TARGET"));

    visualTarget = removePathSeparators(visualTarget);
    QString wrapperTarget = visualTarget + " installer";

    if (installerSisHeader.startsWith("0x", Qt::CaseInsensitive)) {
        tw << sisHeader.arg(wrapperTarget).arg(installerSisHeader).arg(applicationVersion);
    } else {
        tw << installerSisHeader << endl;
    }

    if (headerRules.isEmpty()) {
        t << sisHeader.arg(visualTarget).arg(uid3).arg(applicationVersion);
        ts << sisHeader.arg(visualTarget).arg(uid3).arg(applicationVersion);
    }
    else {
        t << headerRules.join("\n") << endl;
        ts << headerRules.join("\n") << endl;
    }

    // Localized vendor name
    QString vendorName;
    if (!containsStartWithItem('%', rawPkgPreRules)) {
        vendorName += "; Localised Vendor name\n%{\"Vendor\"}\n\n";
    }

    // Unique vendor name
    if (!containsStartWithItem(':', rawPkgPreRules)) {
        vendorName += "; Unique Vendor name\n:\"Vendor\"\n\n";
    }

    t << vendorName;
    tw << vendorName;
    ts << vendorName;

    // PKG pre-rules - these are added before actual file installations i.e. SIS package body
    if (rawPkgPreRules.size()) {
        QString comment = "\n; Manual PKG pre-rules from PRO files\n";
        t << comment;
        tw << comment;
        ts << comment;

        foreach(QString item, rawPkgPreRules) {
            // Only regular pkg file should have package dependencies
            if (item.startsWith("(")) {
                t << item << endl;
            }
            // stub pkg file should not have platform dependencies
            else if (item.startsWith("[")) {
                t << item << endl;
                tw << item << endl;
            }
            else {
                t << item << endl;
                ts << item << endl;
                tw << item << endl;
            }
        }
        t << endl;
        ts << endl;
        tw << endl;
    }

    // Begin Manufacturer block
    if (!project->values("DEPLOYMENT.manufacturers").isEmpty()) {
        QString manufacturerStr("IF ");
        foreach(QString manufacturer, project->values("DEPLOYMENT.manufacturers")) {
            manufacturerStr.append(QString("(MANUFACTURER)=(%1) OR \n   ").arg(manufacturer));
        }
        // Remove the final OR
        manufacturerStr.chop(8);
        t << manufacturerStr << endl;
    }

    // Install paths on the phone *** should be dynamic at some point
    QString installPathBin = "!:\\sys\\bin";
    QString installPathResource = "!:\\resource\\apps";
    QString installPathRegResource = "!:\\private\\10003a3f\\import\\apps";

    // Find location of builds
    QString destDirBin;
    QString destDirResource;
    QString destDirRegResource;
    if (epocBuild) {
        destDirBin = QString("%1epoc32/release/$(PLATFORM)/$(TARGET)").arg(epocRoot());
        destDirResource = QString("%1epoc32/data/z/resource/apps").arg(epocRoot());
        destDirRegResource = QString("%1epoc32/data/z/private/10003a3f/import/apps").arg(epocRoot());
    } else {
        destDirBin = project->first("DESTDIR");
        if (destDirBin.isEmpty())
            destDirBin = ".";
        else if (destDirBin.endsWith('/') || destDirBin.endsWith('\\'))
            destDirBin.chop(1);
        destDirResource = destDirBin;
        destDirRegResource = destDirBin;
    }

    if (targetType == TypeExe) {
        // deploy .exe file
        t << "; Executable and default resource files" << endl;
        QString exeFile = fixedTarget + ".exe";
        t << QString("\"%1/%2\" - \"%3\\%4\"")
             .arg(destDirBin)
             .arg(exeFile)
             .arg(installPathBin)
             .arg(exeFile) << endl;
        ts << QString("\"\" - \"%1\\%2\"")
             .arg(romPath(installPathBin))
             .arg(exeFile) << endl;

        // deploy rsc & reg_rsc file
        if (!project->isActiveConfig("no_icon")) {
            t << QString("\"%1/%2\" - \"%3\\%4\"")
                 .arg(destDirResource)
                 .arg(fixedTarget + ".rsc")
                 .arg(installPathResource)
                 .arg(fixedTarget + ".rsc") << endl;
            ts << QString("\"\" - \"%1\\%2\"")
                 .arg(romPath(installPathResource))
                 .arg(fixedTarget + ".rsc") << endl;

            t << QString("\"%1/%2\" - \"%3\\%4\"")
                 .arg(destDirRegResource)
                 .arg(fixedTarget + "_reg.rsc")
                 .arg(installPathRegResource)
                 .arg(fixedTarget + "_reg.rsc") << endl;
            ts << QString("\"\" - \"%1\\%2\"")
                 .arg(romPath(installPathRegResource))
                 .arg(fixedTarget + "_reg.rsc") << endl;

            if (!iconFile.isEmpty())  {
                if (epocBuild) {
                    t << QString("\"%1epoc32/data/z%2\" - \"!:%3\"")
                         .arg(epocRoot())
                         .arg(iconFile)
                         .arg(QDir::toNativeSeparators(iconFile)) << endl << endl;
                    ts << QString("\"\" - \"%1\"")
                         .arg(romPath(QDir::toNativeSeparators(iconFile))) << endl << endl;
                } else {
                    QDir mifIconDir(project->first("DESTDIR"));
                    QFileInfo mifIcon(mifIconDir.relativeFilePath(project->first("TARGET")));
                    QString mifIconFileName = mifIcon.fileName();
                    mifIconFileName.append(".mif");
                    t << QString("\"%1/%2\" - \"!:%3\"")
                         .arg(mifIcon.path())
                         .arg(mifIconFileName)
                         .arg(QDir::toNativeSeparators(iconFile)) << endl << endl;
                    ts << QString("\"\" - \"%1\"")
                         .arg(romPath(QDir::toNativeSeparators(iconFile))) << endl << endl;
                }
            }
        }
    }

    // deploy any additional DEPLOYMENT  files
    QString remoteTestPath;
    QString zDir;
    remoteTestPath = QString("!:\\private\\%1").arg(privateDirUid);
    if (epocBuild)
        zDir = epocRoot() + QLatin1String("epoc32/data/z");

    DeploymentList depList;
    initProjectDeploySymbian(project, depList, remoteTestPath, true, epocBuild, "$(PLATFORM)", "$(TARGET)", generatedDirs, generatedFiles);
    if (depList.size())
        t << "; DEPLOYMENT" << endl;
    for (int i = 0; i < depList.size(); ++i)  {
        QString from = depList.at(i).from;
        QString to = depList.at(i).to;

        if (epocBuild) {
            // Deploy anything not already deployed from under epoc32 instead from under
            // \epoc32\data\z\ to enable using pkg file without rebuilding
            // the project, which can be useful for some binary only distributions.
            if (!from.contains(QLatin1String("epoc32"), Qt::CaseInsensitive)) {
                from = to;
                if (from.size() > 1 && from.at(1) == QLatin1Char(':'))
                    from = from.mid(2);
                from.prepend(zDir);
            }
        }

        t << QString("\"%1\" - \"%2\"").arg(from.replace('\\','/')).arg(to) << endl;
        ts << QString("\"\" - \"%1\"").arg(romPath(to)) << endl;
    }
    t << endl;
    ts << endl;

    // PKG post-rules - these are added after actual file installations i.e. SIS package body
    t << "; Manual PKG post-rules from PRO files" << endl;
    foreach(QString deploymentItem, project->values("DEPLOYMENT")) {
        foreach(QString pkgrulesItem, project->values(deploymentItem + ".pkg_postrules")) {
            QStringList pkgrulesValue = project->values(pkgrulesItem);
            // If there is no stringlist defined for a rule, use rule name directly
            // This is convenience for defining single line statements
            if (pkgrulesValue.isEmpty()) {
                t << pkgrulesItem << endl;
            } else {
                foreach(QString pkgrule, pkgrulesValue) {
                    t << pkgrule << endl;
                }
            }
            t << endl;
        }
    }

    // Close Manufacturer block
    if (!project->values("DEPLOYMENT.manufacturers").isEmpty()) {
        QString manufacturerFailNoteFile;
        if (project->values("DEPLOYMENT.manufacturers.fail_note").isEmpty()) {
            manufacturerFailNoteFile = QString("%1_" MANUFACTURER_NOTE_FILE).arg(uid3);
            QFile ft(manufacturerFailNoteFile);
            if (ft.open(QIODevice::WriteOnly)) {
                generatedFiles << ft.fileName();
                QTextStream t2(&ft);

                t2 << QString(DEFAULT_MANUFACTURER_NOTE) << endl;
            } else {
                PRINT_FILE_CREATE_ERROR(manufacturerFailNoteFile)
            }
        } else {
            manufacturerFailNoteFile = project->values("DEPLOYMENT.manufacturers.fail_note").join("");
        }

        t << "ELSEIF NOT(0) ; MANUFACTURER" << endl
          << "\"" << generator->fileInfo(manufacturerFailNoteFile).absoluteFilePath() << "\""
          << " - \"\", FILETEXT, TEXTEXIT" << endl
          << "ENDIF ; MANUFACTURER" << endl;
    }

    // Write wrapper pkg
    if (!installerSisHeader.isEmpty()) {
        QFile wrapperPkgFile(wrapperPkgFilename);
        if (!wrapperPkgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            PRINT_FILE_CREATE_ERROR(wrapperPkgFilename);
            return;
        }

        generatedFiles << wrapperPkgFile.fileName();
        QTextStream twf(&wrapperPkgFile);

        twf << wrapperStreamBuffer << endl;

        // Wrapped files deployment
        QString currentPath = qmake_getpwd();
        QString sisName = QString("%1.sis").arg(pkgTarget);
        twf << "\"" << currentPath << "/" << sisName << "\" - \"c:\\private\\2002CCCE\\import\\" << sisName << "\"" << endl;

        QString bootStrapPath = QLibraryInfo::location(QLibraryInfo::PrefixPath);
        bootStrapPath.append("/smartinstaller.sis");
        QFileInfo fi(generator->fileInfo(bootStrapPath));
        twf << "@\"" << fi.absoluteFilePath() << "\",(0x2002CCCD)" << endl;
    }
}

QString SymbianCommonGenerator::removePathSeparators(QString &file)
{
    QString ret = file;

    if (QDir::separator().unicode() != '/')
        ret.replace(QDir::separator(), QLatin1Char('/'));

    if (ret.indexOf(QLatin1Char('/')) > 0)
        ret.remove(0, ret.lastIndexOf(QLatin1Char('/')) + 1);

    return ret;
}

void SymbianCommonGenerator::writeRegRssFile(QMap<QString, QStringList> &userItems)
{
    QString filename(fixedTarget);
    filename.append("_reg.rss");
    if (!Option::output_dir.isEmpty())
        filename = Option::output_dir + '/' + filename;
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();
        QTextStream t(&ft);
        t << "// ============================================================================" << endl;
        t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
        t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
        t << "// * This file is generated by qmake and should not be modified by the" << endl;
        t << "// * user." << endl;
        t << "// ============================================================================" << endl;
        t << endl;
        t << "#include <" << fixedTarget << ".rsg>" << endl;
        t << "#include <appinfo.rh>" << endl;
        foreach(QString item, userItems[RSS_TAG_HEADER])
            t << item << endl;
        t << endl;
        t << "UID2 KUidAppRegistrationResourceFile" << endl;
        t << "UID3 " << uid3 << endl << endl;
        t << "RESOURCE APP_REGISTRATION_INFO" << endl;
        t << "\t{" << endl;
        t << "\tapp_file=\"" << fixedTarget << "\";" << endl;
        t << "\tlocalisable_resource_file=\"" RESOURCE_DIRECTORY_RESOURCE << fixedTarget << "\";" << endl;

        writeRegRssList(t, userItems[RSS_TAG_SERVICE_LIST],
                        QLatin1String(RSS_TAG_SERVICE_LIST),
                        QLatin1String("SERVICE_INFO"));
        writeRegRssList(t, userItems[RSS_TAG_FILE_OWNERSHIP_LIST],
                        QLatin1String(RSS_TAG_FILE_OWNERSHIP_LIST),
                        QLatin1String("FILE_OWNERSHIP_INFO"));
        writeRegRssList(t, userItems[RSS_TAG_DATATYPE_LIST],
                        QLatin1String(RSS_TAG_DATATYPE_LIST),
                        QLatin1String("DATATYPE"));
        t << endl;

        foreach(QString item, userItems[RSS_TAG_DEFAULT])
            t << "\t" << item.replace("\n","\n\t") << endl;
        t << "\t}" << endl;

        foreach(QString item, userItems[RSS_TAG_FOOTER])
            t << item << endl;
    } else {
        PRINT_FILE_CREATE_ERROR(filename)
    }
}

void SymbianCommonGenerator::writeRegRssList(QTextStream &t,
                                               QStringList &userList,
                                               const QString &listTag,
                                               const QString &listItem)
{
    int itemCount = userList.count();
    if (itemCount) {
        t << "\t" << listTag << " ="<< endl;
        t << "\t\t{" << endl;
        foreach(QString item, userList) {
            t << "\t\t" << listItem << endl;
            t << "\t\t\t{" << endl;
            t << "\t\t\t" << item.replace("\n","\n\t\t\t") << endl;
            t << "\t\t\t}";
            if (--itemCount)
                t << ",";
            t << endl;
        }
        t << "\t\t}; "<< endl;
    }
}

void SymbianCommonGenerator::writeRssFile(QString &numberOfIcons, QString &iconFile)
{
    QString filename(fixedTarget);
    if (!Option::output_dir.isEmpty())
        filename = Option::output_dir + '/' + filename;
    filename.append(".rss");
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();
        QTextStream t(&ft);
        t << "// ============================================================================" << endl;
        t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
        t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
        t << "// * This file is generated by qmake and should not be modified by the" << endl;
        t << "// * user." << endl;
        t << "// ============================================================================" << endl;
        t << endl;
        t << "#include <appinfo.rh>" << endl;
        t << "#include \"" << fixedTarget << ".loc\"" << endl;
        t << endl;
        t << "RESOURCE LOCALISABLE_APP_INFO r_localisable_app_info" << endl;
        t << "\t{" << endl;
        t << "\tshort_caption = STRING_r_short_caption;" << endl;
        t << "\tcaption_and_icon =" << endl;
        t << "\tCAPTION_AND_ICON_INFO" << endl;
        t << "\t\t{" << endl;
        t << "\t\tcaption = STRING_r_caption;" << endl;

        QString rssIconFile = iconFile;
        rssIconFile = rssIconFile.replace("/", "\\\\");

        if (numberOfIcons.isEmpty() || rssIconFile.isEmpty()) {
            // There can be maximum one item in this tag, validated when parsed
            t << "\t\tnumber_of_icons = 0;" << endl;
            t << "\t\ticon_file = \"\";" << endl;
        } else {
            // There can be maximum one item in this tag, validated when parsed
            t << "\t\tnumber_of_icons = " << numberOfIcons << ";" << endl;
            t << "\t\ticon_file = \"" << rssIconFile << "\";" << endl;
        }
        t << "\t\t};" << endl;
        t << "\t}" << endl;
        t << endl;
    } else {
        PRINT_FILE_CREATE_ERROR(filename);
    }
}

void SymbianCommonGenerator::writeLocFile(QStringList &symbianLangCodes)
{
    QString filename(fixedTarget);
    if (!Option::output_dir.isEmpty())
        filename = Option::output_dir + '/' + filename;
    filename.append(".loc");
    QFile ft(filename);
    if (ft.open(QIODevice::WriteOnly)) {
        generatedFiles << ft.fileName();
        QTextStream t(&ft);
        t << "// ============================================================================" << endl;
        t << "// * Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
        t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
        t << "// * This file is generated by qmake and should not be modified by the" << endl;
        t << "// * user." << endl;
        t << "// ============================================================================" << endl;
        t << endl;
        t << "#ifdef LANGUAGE_SC" << endl;
        t << "#define STRING_r_short_caption \"" << fixedTarget  << "\"" << endl;
        t << "#define STRING_r_caption \"" << fixedTarget  << "\"" << endl;
        foreach(QString lang, symbianLangCodes) {
            t << "#elif defined LANGUAGE_" << lang << endl;
            t << "#define STRING_r_short_caption \"" << fixedTarget  << "\"" << endl;
            t << "#define STRING_r_caption \"" << fixedTarget  << "\"" << endl;
        }
        t << "#else" << endl;
        t << "#define STRING_r_short_caption \"" << fixedTarget  << "\"" << endl;
        t << "#define STRING_r_caption \"" << fixedTarget  << "\"" << endl;
        t << "#endif" << endl;
    } else {
        PRINT_FILE_CREATE_ERROR(filename);
    }
}

void SymbianCommonGenerator::readRssRules(QString &numberOfIcons,
                                            QString &iconFile, QMap<QString,
                                            QStringList> &userRssRules)
{
    QMakeProject *project = generator->project;
    for (QMap<QString, QStringList>::iterator it = project->variables().begin(); it != project->variables().end(); ++it) {
        if (it.key().startsWith(RSS_RULES_BASE)) {
            QString newKey = it.key().mid(sizeof(RSS_RULES_BASE) - 1);
            if (newKey.isEmpty()) {
                fprintf(stderr, "Warning: Empty RSS_RULES_BASE key encountered\n");
                continue;
            }
            QStringList newValues;
            QStringList values = it.value();
            foreach(QString item, values) {
                // If there is no stringlist defined for a rule, use rule value directly
                // This is convenience for defining single line statements
                if (project->values(item).isEmpty()) {
                    newValues << item;
                } else {
                    QStringList itemList;
                    foreach(QString itemRow, project->values(item)) {
                        itemList << itemRow;
                    }
                    newValues << itemList.join("\n");
                }
            }
            // Verify thet there is exactly one value in RSS_TAG_NBROFICONS
            if (newKey == RSS_TAG_NBROFICONS) {
                if (newValues.count() == 1) {
                    numberOfIcons = newValues[0];
                } else {
                    fprintf(stderr, "Warning: There must be exactly one value in '%s%s'\n",
                            RSS_RULES_BASE, RSS_TAG_NBROFICONS);
                    continue;
                }
            // Verify thet there is exactly one value in RSS_TAG_ICONFILE
            } else if (newKey == RSS_TAG_ICONFILE) {
                if (newValues.count() == 1) {
                    iconFile = newValues[0];
                } else {
                    fprintf(stderr, "Warning: There must be exactly one value in '%s%s'\n",
                            RSS_RULES_BASE, RSS_TAG_ICONFILE);
                    continue;
                }
            } else if (newKey == RSS_TAG_HEADER
                       || newKey == RSS_TAG_SERVICE_LIST
                       || newKey == RSS_TAG_FILE_OWNERSHIP_LIST
                       || newKey == RSS_TAG_DATATYPE_LIST
                       || newKey == RSS_TAG_FOOTER
                       || newKey == RSS_TAG_DEFAULT) {
                userRssRules[newKey] = newValues;
                continue;
            } else {
                fprintf(stderr, "Warning: Unsupported key:'%s%s'\n",
                        RSS_RULES_BASE, newKey.toLatin1().constData());
                continue;
            }
        }
    }

    QStringList newValues;
    foreach(QString item, project->values(RSS_RULES)) {
        // If there is no stringlist defined for a rule, use rule value directly
        // This is convenience for defining single line statements
        if (project->values(item).isEmpty()) {
            newValues << item;
        } else {
            newValues << project->values(item);
        }
    }
    userRssRules[RSS_TAG_DEFAULT] << newValues;

    // Validate that either both RSS_TAG_NBROFICONS and RSS_TAG_ICONFILE keys exist
    // or neither of them exist
    if (!((numberOfIcons.isEmpty() && iconFile.isEmpty()) ||
            (!numberOfIcons.isEmpty() && !iconFile.isEmpty()))) {
        numberOfIcons.clear();
        iconFile.clear();
        fprintf(stderr, "Warning: Both or neither of '%s%s' and '%s%s' keys must exist.\n",
                RSS_RULES_BASE, RSS_TAG_NBROFICONS, RSS_RULES_BASE, RSS_TAG_ICONFILE);
    }

    // Validate that RSS_TAG_NBROFICONS contains only numbers
    if (!numberOfIcons.isEmpty()) {
        bool ok;
        numberOfIcons = numberOfIcons.simplified();
        numberOfIcons.toInt(&ok);
        if (!ok) {
            numberOfIcons.clear();
            iconFile.clear();
            fprintf(stderr, "Warning: '%s%s' must be integer in decimal format.\n",
                    RSS_RULES_BASE, RSS_TAG_NBROFICONS);
        }
    }
}

void SymbianCommonGenerator::writeCustomDefFile()
{
    if (targetType == TypePlugin && !generator->project->isActiveConfig("stdbinary")) {
        // Create custom def file for plugin
        QFile ft(Option::output_dir + QLatin1Char('/') + QLatin1String(PLUGIN_COMMON_DEF_FILE_ACTUAL));

        if (ft.open(QIODevice::WriteOnly)) {
            generatedFiles << ft.fileName();
            QTextStream t(&ft);

            t << "; ==============================================================================" << endl;
            t << "; Generated by qmake (" << qmake_version() << ") (Qt " QT_VERSION_STR ") on: ";
            t << QDateTime::currentDateTime().toString(Qt::ISODate) << endl;
            t << "; This file is generated by qmake and should not be modified by the" << endl;
            t << "; user." << endl;
            t << ";  Name        : " PLUGIN_COMMON_DEF_FILE_ACTUAL << endl;
            t << ";  Part of     : " << generator->project->values("TARGET").join(" ") << endl;
            t << ";  Description : Fixes common plugin symbols to known ordinals" << endl;
            t << ";  Version     : " << endl;
            t << ";" << endl;
            t << "; ==============================================================================" << "\n" << endl;

            t << endl;

            t << "EXPORTS" << endl;
            t << "\tqt_plugin_query_verification_data @ 1 NONAME" << endl;
            t << "\tqt_plugin_instance @ 2 NONAME" << endl;
            t << endl;
        } else {
            PRINT_FILE_CREATE_ERROR(QString(PLUGIN_COMMON_DEF_FILE_ACTUAL))
        }
    }
}

QStringList SymbianCommonGenerator::symbianLangCodesFromTsFiles()
{
    QStringList tsfiles;
    QStringList symbianLangCodes;
    tsfiles << generator->project->values("TRANSLATIONS");

    fillQt2S60LangMapTable();

    foreach(QString file, tsfiles) {
        int extIndex = file.lastIndexOf(".");
        int langIndex = file.lastIndexOf("_", (extIndex - file.length()));
        langIndex += 1;
        QString qtlang = file.mid(langIndex, extIndex - langIndex);
        QString s60lang = qt2S60LangMapTable.value(qtlang, QString("SC"));

        if (!symbianLangCodes.contains(s60lang) && s60lang != "SC")
            symbianLangCodes += s60lang;
    }

    return symbianLangCodes;
}

void SymbianCommonGenerator::fillQt2S60LangMapTable()
{
    qt2S60LangMapTable.reserve(170); // 165 items at time of writing.
    qt2S60LangMapTable.insert("ab", "SC");            //Abkhazian                     //
    qt2S60LangMapTable.insert("om", "SC");            //Afan                          //
    qt2S60LangMapTable.insert("aa", "SC");            //Afar                          //
    qt2S60LangMapTable.insert("af", "34");            //Afrikaans                     //Afrikaans
    qt2S60LangMapTable.insert("sq", "35");            //Albanian                      //Albanian
    qt2S60LangMapTable.insert("am", "36");            //Amharic                       //Amharic
    qt2S60LangMapTable.insert("ar", "37");            //Arabic                        //Arabic
    qt2S60LangMapTable.insert("hy", "38");            //Armenian                      //Armenian
    qt2S60LangMapTable.insert("as", "SC");            //Assamese                      //
    qt2S60LangMapTable.insert("ay", "SC");            //Aymara                        //
    qt2S60LangMapTable.insert("az", "SC");            //Azerbaijani                   //
    qt2S60LangMapTable.insert("ba", "SC");            //Bashkir                       //
    qt2S60LangMapTable.insert("eu", "SC");            //Basque                        //
    qt2S60LangMapTable.insert("bn", "41");            //Bengali                       //Bengali
    qt2S60LangMapTable.insert("dz", "SC");            //Bhutani                       //
    qt2S60LangMapTable.insert("bh", "SC");            //Bihari                        //
    qt2S60LangMapTable.insert("bi", "SC");            //Bislama                       //
    qt2S60LangMapTable.insert("br", "SC");            //Breton                        //
    qt2S60LangMapTable.insert("bg", "42");            //Bulgarian                     //Bulgarian
    qt2S60LangMapTable.insert("my", "43");            //Burmese                       //Burmese
    qt2S60LangMapTable.insert("be", "40");            //Byelorussian                  //Belarussian
    qt2S60LangMapTable.insert("km", "SC");            //Cambodian                     //
    qt2S60LangMapTable.insert("ca", "44");            //Catalan                       //Catalan
    qt2S60LangMapTable.insert("zh", "SC");            //Chinese                       //
    qt2S60LangMapTable.insert("co", "SC");            //Corsican                      //
    qt2S60LangMapTable.insert("hr", "45");            //Croatian                      //Croatian
    qt2S60LangMapTable.insert("cs", "25");            //Czech                         //Czech
    qt2S60LangMapTable.insert("da", "07");            //Danish                        //Danish
    qt2S60LangMapTable.insert("nl", "18");            //Dutch                         //Dutch
    qt2S60LangMapTable.insert("en", "01");            //English                       //English(UK)
    qt2S60LangMapTable.insert("eo", "SC");            //Esperanto                     //
    qt2S60LangMapTable.insert("et", "49");            //Estonian                      //Estonian
    qt2S60LangMapTable.insert("fo", "SC");            //Faroese                       //
    qt2S60LangMapTable.insert("fj", "SC");            //Fiji                          //
    qt2S60LangMapTable.insert("fi", "09");            //Finnish                       //Finnish
    qt2S60LangMapTable.insert("fr", "02");            //French                        //French
    qt2S60LangMapTable.insert("fy", "SC");            //Frisian                       //
    qt2S60LangMapTable.insert("gd", "52");            //Gaelic                        //Gaelic
    qt2S60LangMapTable.insert("gl", "SC");            //Galician                      //
    qt2S60LangMapTable.insert("ka", "53");            //Georgian                      //Georgian
    qt2S60LangMapTable.insert("de", "03");            //German                        //German
    qt2S60LangMapTable.insert("el", "54");            //Greek                         //Greek
    qt2S60LangMapTable.insert("kl", "SC");            //Greenlandic                   //
    qt2S60LangMapTable.insert("gn", "SC");            //Guarani                       //
    qt2S60LangMapTable.insert("gu", "56");            //Gujarati                      //Gujarati
    qt2S60LangMapTable.insert("ha", "SC");            //Hausa                         //
    qt2S60LangMapTable.insert("he", "57");            //Hebrew                        //Hebrew
    qt2S60LangMapTable.insert("hi", "58");            //Hindi                         //Hindi
    qt2S60LangMapTable.insert("hu", "17");            //Hungarian                     //Hungarian
    qt2S60LangMapTable.insert("is", "15");            //Icelandic                     //Icelandic
    qt2S60LangMapTable.insert("id", "59");            //Indonesian                    //Indonesian
    qt2S60LangMapTable.insert("ia", "SC");            //Interlingua                   //
    qt2S60LangMapTable.insert("ie", "SC");            //Interlingue                   //
    qt2S60LangMapTable.insert("iu", "SC");            //Inuktitut                     //
    qt2S60LangMapTable.insert("ik", "SC");            //Inupiak                       //
    qt2S60LangMapTable.insert("ga", "60");            //Irish                         //Irish
    qt2S60LangMapTable.insert("it", "05");            //Italian                       //Italian
    qt2S60LangMapTable.insert("ja", "32");            //Japanese                      //Japanese
    qt2S60LangMapTable.insert("jv", "SC");            //Javanese                      //
    qt2S60LangMapTable.insert("kn", "62");            //Kannada                       //Kannada
    qt2S60LangMapTable.insert("ks", "SC");            //Kashmiri                      //
    qt2S60LangMapTable.insert("kk", "63");            //Kazakh                        //Kazakh
    qt2S60LangMapTable.insert("rw", "SC");            //Kinyarwanda                   //
    qt2S60LangMapTable.insert("ky", "SC");            //Kirghiz                       //
    qt2S60LangMapTable.insert("ko", "65");            //Korean                        //Korean
    qt2S60LangMapTable.insert("ku", "SC");            //Kurdish                       //
    qt2S60LangMapTable.insert("rn", "SC");            //Kurundi                       //
    qt2S60LangMapTable.insert("lo", "66");            //Laothian                      //Laothian
    qt2S60LangMapTable.insert("la", "SC");            //Latin                         //
    qt2S60LangMapTable.insert("lv", "67");            //Latvian                       //Latvian
    qt2S60LangMapTable.insert("ln", "SC");            //Lingala                       //
    qt2S60LangMapTable.insert("lt", "68");            //Lithuanian                    //Lithuanian
    qt2S60LangMapTable.insert("mk", "69");            //Macedonian                    //Macedonian
    qt2S60LangMapTable.insert("mg", "SC");            //Malagasy                      //
    qt2S60LangMapTable.insert("ms", "70");            //Malay                         //Malay
    qt2S60LangMapTable.insert("ml", "71");            //Malayalam                     //Malayalam
    qt2S60LangMapTable.insert("mt", "SC");            //Maltese                       //
    qt2S60LangMapTable.insert("mi", "SC");            //Maori                         //
    qt2S60LangMapTable.insert("mr", "72");            //Marathi                       //Marathi
    qt2S60LangMapTable.insert("mo", "73");            //Moldavian                     //Moldovian
    qt2S60LangMapTable.insert("mn", "74");            //Mongolian                     //Mongolian
    qt2S60LangMapTable.insert("na", "SC");            //Nauru                         //
    qt2S60LangMapTable.insert("ne", "SC");            //Nepali                        //
    qt2S60LangMapTable.insert("nb", "08");            //Norwegian                     //Norwegian
    qt2S60LangMapTable.insert("oc", "SC");            //Occitan                       //
    qt2S60LangMapTable.insert("or", "SC");            //Oriya                         //
    qt2S60LangMapTable.insert("ps", "SC");            //Pashto                        //
    qt2S60LangMapTable.insert("fa", "SC");            //Persian                       //
    qt2S60LangMapTable.insert("pl", "27");            //Polish                        //Polish
    qt2S60LangMapTable.insert("pt", "13");            //Portuguese                    //Portuguese
    qt2S60LangMapTable.insert("pa", "77");            //Punjabi                       //Punjabi
    qt2S60LangMapTable.insert("qu", "SC");            //Quechua                       //
    qt2S60LangMapTable.insert("rm", "SC");            //RhaetoRomance                 //
    qt2S60LangMapTable.insert("ro", "78");            //Romanian                      //Romanian
    qt2S60LangMapTable.insert("ru", "16");            //Russian                       //Russian
    qt2S60LangMapTable.insert("sm", "SC");            //Samoan                        //
    qt2S60LangMapTable.insert("sg", "SC");            //Sangho                        //
    qt2S60LangMapTable.insert("sa", "SC");            //Sanskrit                      //
    qt2S60LangMapTable.insert("sr", "79");            //Serbian                       //Serbian
    qt2S60LangMapTable.insert("sh", "SC");            //SerboCroatian                 //
    qt2S60LangMapTable.insert("st", "SC");            //Sesotho                       //
    qt2S60LangMapTable.insert("tn", "SC");            //Setswana                      //
    qt2S60LangMapTable.insert("sn", "SC");            //Shona                         //
    qt2S60LangMapTable.insert("sd", "SC");            //Sindhi                        //
    qt2S60LangMapTable.insert("si", "80");            //Singhalese                    //Sinhalese
    qt2S60LangMapTable.insert("ss", "SC");            //Siswati                       //
    qt2S60LangMapTable.insert("sk", "26");            //Slovak                        //Slovak
    qt2S60LangMapTable.insert("sl", "28");            //Slovenian                     //Slovenian
    qt2S60LangMapTable.insert("so", "81");            //Somali                        //Somali
    qt2S60LangMapTable.insert("es", "04");            //Spanish                       //Spanish
    qt2S60LangMapTable.insert("su", "SC");            //Sundanese                     //
    qt2S60LangMapTable.insert("sw", "84");            //Swahili                       //Swahili
    qt2S60LangMapTable.insert("sv", "06");            //Swedish                       //Swedish
    qt2S60LangMapTable.insert("tl", "39");            //Tagalog                       //Tagalog
    qt2S60LangMapTable.insert("tg", "SC");            //Tajik                         //
    qt2S60LangMapTable.insert("ta", "87");            //Tamil                         //Tamil
    qt2S60LangMapTable.insert("tt", "SC");            //Tatar                         //
    qt2S60LangMapTable.insert("te", "88");            //Telugu                        //Telugu
    qt2S60LangMapTable.insert("th", "33");            //Thai                          //Thai
    qt2S60LangMapTable.insert("bo", "89");            //Tibetan                       //Tibetan
    qt2S60LangMapTable.insert("ti", "90");            //Tigrinya                      //Tigrinya
    qt2S60LangMapTable.insert("to", "SC");            //Tonga                         //
    qt2S60LangMapTable.insert("ts", "SC");            //Tsonga                        //
    qt2S60LangMapTable.insert("tr", "14");            //Turkish                       //Turkish
    qt2S60LangMapTable.insert("tk", "92");            //Turkmen                       //Turkmen
    qt2S60LangMapTable.insert("tw", "SC");            //Twi                           //
    qt2S60LangMapTable.insert("ug", "SC");            //Uigur                         //
    qt2S60LangMapTable.insert("uk", "93");            //Ukrainian                     //Ukrainian
    qt2S60LangMapTable.insert("ur", "94");            //Urdu                          //Urdu
    qt2S60LangMapTable.insert("uz", "SC");            //Uzbek                         //
    qt2S60LangMapTable.insert("vi", "96");            //Vietnamese                    //Vietnamese
    qt2S60LangMapTable.insert("vo", "SC");            //Volapuk                       //
    qt2S60LangMapTable.insert("cy", "97");            //Welsh                         //Welsh
    qt2S60LangMapTable.insert("wo", "SC");            //Wolof                         //
    qt2S60LangMapTable.insert("xh", "SC");            //Xhosa                         //
    qt2S60LangMapTable.insert("yi", "SC");            //Yiddish                       //
    qt2S60LangMapTable.insert("yo", "SC");            //Yoruba                        //
    qt2S60LangMapTable.insert("za", "SC");            //Zhuang                        //
    qt2S60LangMapTable.insert("zu", "98");            //Zulu                          //Zulu
    qt2S60LangMapTable.insert("nn", "75");            //Nynorsk                       //NorwegianNynorsk
    qt2S60LangMapTable.insert("bs", "SC");            //Bosnian                       //
    qt2S60LangMapTable.insert("dv", "SC");            //Divehi                        //
    qt2S60LangMapTable.insert("gv", "SC");            //Manx                          //
    qt2S60LangMapTable.insert("kw", "SC");            //Cornish                       //
    qt2S60LangMapTable.insert("ak", "SC");            //Akan                          //
    qt2S60LangMapTable.insert("kok", "SC");           //Konkani                       //
    qt2S60LangMapTable.insert("gaa", "SC");           //Ga                            //
    qt2S60LangMapTable.insert("ig", "SC");            //Igbo                          //
    qt2S60LangMapTable.insert("kam", "SC");           //Kamba                         //
    qt2S60LangMapTable.insert("syr", "SC");           //Syriac                        //
    qt2S60LangMapTable.insert("byn", "SC");           //Blin                          //
    qt2S60LangMapTable.insert("gez", "SC");           //Geez                          //
    qt2S60LangMapTable.insert("kfo", "SC");           //Koro                          //
    qt2S60LangMapTable.insert("sid", "SC");           //Sidamo                        //
    qt2S60LangMapTable.insert("cch", "SC");           //Atsam                         //
    qt2S60LangMapTable.insert("tig", "SC");           //Tigre                         //
    qt2S60LangMapTable.insert("kaj", "SC");           //Jju                           //
    qt2S60LangMapTable.insert("fur", "SC");           //Friulian                      //
    qt2S60LangMapTable.insert("ve", "SC");            //Venda                         //
    qt2S60LangMapTable.insert("ee", "SC");            //Ewe                           //
    qt2S60LangMapTable.insert("wa", "SC");            //Walamo                        //
    qt2S60LangMapTable.insert("haw", "SC");           //Hawaiian                      //
    qt2S60LangMapTable.insert("kcg", "SC");           //Tyap                          //
    qt2S60LangMapTable.insert("ny", "SC");            //Chewa                         //
}

