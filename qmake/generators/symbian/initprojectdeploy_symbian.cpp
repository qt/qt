/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $TROLLTECH_DUAL_LICENSE$
**
****************************************************************************/

#include "initprojectdeploy_symbian.h"
#include <QDirIterator>
#include <project.h>
#include <qdebug.h>

#define PLUGIN_STUB_DIR "qmakepluginstubs"

static bool isPlugin(const QFileInfo& info, const QString& devicePath)
{
    // Libraries are plugins if deployment path is something else than
    // \\sys\\bin or x:\\sys\\bin
    if (0 == info.suffix().compare(QLatin1String("dll")) &&
        (devicePath.size() < 8 ||
         (0 != devicePath.compare(QLatin1String("\\sys\\bin")) &&
          0 != devicePath.mid(1).compare(QLatin1String(":\\sys\\bin"))))) {
        return true;
    } else {
        return false;
    }
}

static bool isBinary(const QFileInfo& info)
{
    if (0 == info.suffix().compare(QLatin1String("dll")) ||
        0 == info.suffix().compare(QLatin1String("exe"))) {
        return true;
    } else {
        return false;
    }
}

static void createPluginStub(const QFileInfo& info,
                             const QString& devicePath,
                             DeploymentList &deploymentList,
                             QStringList& generatedDirs,
                             QStringList& generatedFiles) {
    QDir().mkpath(QLatin1String(PLUGIN_STUB_DIR "\\"));
    if (!generatedDirs.contains(PLUGIN_STUB_DIR))
        generatedDirs << PLUGIN_STUB_DIR;
    // Plugin stubs must have different name from the actual plugins, because
    // the toolchain for creating ROM images cannot handle non-binary .dll files properly.
    QFile stubFile(QLatin1String(PLUGIN_STUB_DIR "\\") + info.completeBaseName() + ".qtplugin");
    if(stubFile.open(QIODevice::WriteOnly)) {
        if (!generatedFiles.contains(stubFile.fileName()))
            generatedFiles << stubFile.fileName();
        QTextStream t(&stubFile);
        // Add note to stub so that people will not wonder what it is.
        // Creation date is added to make new stub to deploy always to
        // force plugin cache miss when loading plugins.
        t << "This file is a Qt plugin stub file. The real Qt plugin is located in \\sys\\bin. Created:" << QDateTime::currentDateTime().toString() << "\n";
    } else {
        fprintf(stderr, "cannot deploy \"%s\" because of plugin stub file creation failed\n", info.fileName().toLatin1().constData());
    }
    QFileInfo stubInfo(stubFile);
    deploymentList.append(CopyItem(Option::fixPathToLocalOS(stubInfo.absoluteFilePath()),
        Option::fixPathToLocalOS(devicePath + "\\" + stubInfo.fileName())));
}

void initProjectDeploySymbian(QMakeProject* project,
                              DeploymentList &deploymentList,
                              const QString &testPath,
                              bool deployBinaries,
                              const QString &platform,
                              const QString &build,
                              QStringList& generatedDirs,
                              QStringList& generatedFiles)
{
    QString targetPath = project->values("deploy.path").join(" ");
    if (targetPath.isEmpty())
        targetPath = testPath;
    if (targetPath.endsWith("/") || targetPath.endsWith("\\"))
        targetPath = targetPath.mid(0,targetPath.size()-1);

    bool targetPathHasDriveLetter = false;
    if (targetPath.size() > 1) {
        targetPathHasDriveLetter = targetPath.at(1) == QLatin1Char(':');
    }
    QString deploymentDrive = targetPathHasDriveLetter ? targetPath.left(2) : QLatin1String("c:");

    // foreach item in DEPLOYMENT
    foreach(QString item, project->values("DEPLOYMENT")) {
        // get item.path
        QString devicePath = project->first(item + ".path");
        if (!deployBinaries
            && !devicePath.isEmpty()
            && (0 == devicePath.compare(project->values("APP_RESOURCE_DIR").join(""))
              || 0 == devicePath.compare(project->values("REG_RESOURCE_IMPORT_DIR").join("")))) {
            // Do not deploy resources in emulator builds, as that seems to cause conflicts
            // If there is ever a real need to deploy pre-built resources for emulator,
            // BLD_INF_RULES.prj_exports can be used as a workaround.
            continue;
        }

        bool devicePathHasDriveLetter = false;
        if (devicePath.size() > 1) {
            devicePathHasDriveLetter = devicePath.at(1) == QLatin1Char(':');
        }

        if (devicePath.isEmpty() || devicePath == QLatin1String(".")) {
            devicePath = targetPath;
        }
        // check if item.path is relative (! either / or \)
        else if (!(devicePath.at(0) == QLatin1Char('/')
            || devicePath.at(0) == QLatin1Char('\\')
            || devicePathHasDriveLetter)) {
            // create output path
            devicePath = Option::fixPathToLocalOS(QDir::cleanPath(targetPath + QLatin1Char('\\') + devicePath));
        } else {
            if (0 == platform.compare(QLatin1String("winscw"))) {
                devicePath = epocRoot() + "epoc32\\winscw\\c" + devicePath;
            } else {
                // Drive letter needed if targetpath contains one and it is not already in
                if (targetPathHasDriveLetter && !devicePathHasDriveLetter) {
                    devicePath = deploymentDrive + devicePath;
                }
            }
        }

        devicePath.replace(QLatin1String("/"), QLatin1String("\\"));

        if (!deployBinaries &&
            0 == devicePath.right(8).compare(QLatin1String("\\sys\\bin"))) {
                // Skip deploying to \\sys\\bin for anything but binary deployments
                // Note: Deploying pre-built binaries also follow this rule, so emulator builds
                // will not get those deployed. Since there is no way to differentiate currently
                // between pre-built binaries for emulator and HW anyway, this is not a major issue.
                continue;
            }

        // foreach d in item.sources
        foreach(QString source, project->values(item + ".sources")) {
            source = Option::fixPathToLocalOS(source);
            QString nameFilter;
            QFileInfo info(source);
            QString searchPath;
            bool dirSearch = false;

            if (info.isDir()) {
                nameFilter = QLatin1String("*");
                searchPath = info.absoluteFilePath();
                dirSearch = true;
            } else {
                if (info.exists() || source.indexOf('*') != -1) {
                    nameFilter = source.split('\\').last();
                    searchPath = info.absolutePath();
                } else {
                    // Entry was not found. That is ok if it is a binary, since those do not necessarily yet exist.
                    // Dlls need to be processed even when not deploying binaries for the stubs
                    if (isBinary(info)) {
                        if (deployBinaries) {
                            // Executables and libraries are deployed to \sys\bin
                            QFileInfo releasePath(epocRoot() + "epoc32\\release\\" + platform + "\\" + build + "\\");
                            deploymentList.append(CopyItem(Option::fixPathToLocalOS(releasePath.absolutePath() + "\\" + info.fileName()),
                                Option::fixPathToLocalOS(deploymentDrive + QLatin1String("\\sys\\bin\\") + info.fileName())));
                        }
                        if (isPlugin(info, devicePath)) {
                            createPluginStub(info, devicePath, deploymentList, generatedDirs, generatedFiles);
                            continue;
                        }
                    } else {
                        // Generate deployment even if file doesn't exist, as this may be the case
                        // when generating .pkg files.
                        deploymentList.append(CopyItem(Option::fixPathToLocalOS(info.absoluteFilePath()),
                            Option::fixPathToLocalOS(devicePath + "\\" + info.fileName())));
                        continue;
                    }
                }
            }

            int pathSize = info.absolutePath().size();
            QDirIterator iterator(searchPath, QStringList() << nameFilter
                                  , QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks
                                  , dirSearch ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags );
            // foreach dirIterator-entry in d
            while(iterator.hasNext()) {
                iterator.next();
                QFileInfo iteratorInfo(iterator.filePath());
                QString absoluteItemPath = Option::fixPathToLocalOS(iteratorInfo.absolutePath());
                int diffSize = absoluteItemPath.size() - pathSize;

                if (!iteratorInfo.isDir()) {
                    if (isPlugin(iterator.fileInfo(), devicePath)) {
                        // This deploys pre-built plugins. Other pre-built binaries will deploy normally,
                        // as they have \sys\bin target path.
                        if (deployBinaries) {
                            deploymentList.append(CopyItem(Option::fixPathToLocalOS(absoluteItemPath + "\\" + iterator.fileName()),
                                Option::fixPathToLocalOS(deploymentDrive + QLatin1String("\\sys\\bin\\") + iterator.fileName())));
                        }
                        createPluginStub(info, devicePath + "\\" + absoluteItemPath.right(diffSize), deploymentList, generatedDirs, generatedFiles);
                        continue;
                    } else {
                        deploymentList.append(CopyItem(Option::fixPathToLocalOS(absoluteItemPath + "\\" + iterator.fileName()),
                            Option::fixPathToLocalOS(devicePath + "\\" + absoluteItemPath.right(diffSize) + "\\" + iterator.fileName())));
                    }
                }
            }
        }
    }
}
