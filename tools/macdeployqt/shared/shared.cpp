/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QString>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include <QProcess>
#include <QDir>
#include <QRegExp>
#include <QSet>
#include <QDirIterator>
#include "shared.h"

bool runStripEnabled = true;
int logLevel = 1;

using std::cout;
using std::endl;

bool operator==(const FrameworkInfo &a, const FrameworkInfo &b)
{
    return ((a.frameworkPath == b.frameworkPath) && (a.binaryPath == b.binaryPath));
}

QDebug operator<<(QDebug debug, const FrameworkInfo &info)
{
    debug << "Framework name" << info.frameworkName << "\n";
    debug << "Framework directory" << info.frameworkDirectory << "\n";
    debug << "Framework path" << info.frameworkPath << "\n";
    debug << "Binary directory" << info.binaryDirectory << "\n";
    debug << "Binary name" << info.binaryName << "\n";
    debug << "Binary path" << info.binaryPath << "\n";
    debug << "Version" << info.version << "\n";
    debug << "Install name" << info.installName << "\n";
    debug << "Deployed install name" << info.deployedInstallName << "\n";
    debug << "Source file Path" << info.sourceFilePath << "\n";
    debug << "Destination Directory (relative to bundle)" << info.destinationDirectory << "\n";

    return debug;
}

const QString bundleFrameworkDirectory = "Contents/Frameworks";
const QString bundleBinaryDirectory = "Contents/MacOS";

inline QDebug operator<<(QDebug debug, const ApplicationBundleInfo &info)
{
    debug << "Application bundle path" << info.path << "\n";
    debug << "Binary path" << info.binaryPath << "\n";
    debug << "Additional libraries" << info.libraryPaths << "\n";
    return debug;
}

bool copyFilePrintStatus(const QString &from, const QString &to)
{
    if (QFile::copy(from, to)) {
        LogNormal() << " copied:" << from;
        LogNormal() << " to" << to;
        return true;
    } else {
        LogError() << "file copy failed from" << from;
        LogError() << " to" << to;
        return false;
    }
}

FrameworkInfo parseOtoolLibraryLine(const QString &line, bool useDebugLibs)
{
    FrameworkInfo info;
    QString trimmed = line.trimmed();

    if (trimmed.isEmpty())
        return info;

    // Don't deploy system libraries.
    if (trimmed.startsWith("/System/Library/") ||
        (trimmed.startsWith("/usr/lib/") && trimmed.contains("libQt") == false) // exception for libQtuitools and libQtlucene
        || trimmed.startsWith("@executable_path") || trimmed.startsWith("@loader_path") || trimmed.startsWith("@rpath"))
        return info;

    enum State {QtPath, FrameworkName, DylibName, Version, End};
    State state = QtPath;
    int part = 0;
    QString name;
    QString qtPath;
    QString suffix = useDebugLibs ? "_debug" : "";

    // Split the line into [Qt-path]/lib/qt[Module].framework/Versions/[Version]/
    QStringList parts = trimmed.split("/");
    while (part < parts.count()) {
        const QString currentPart = parts.at(part).simplified() ;
        ++part;
        if (currentPart.isEmpty())
            continue;

        if (state == QtPath) {
            // Check for library name part
            if (part < parts.count() && parts.at(part).contains(".dylib ")) {
                state = DylibName;
                info.installName += "/" + (qtPath + "lib/").simplified();
                info.frameworkDirectory = info.installName;
                state = DylibName;
                continue;
            } else if (part < parts.count() && parts.at(part).endsWith(".framework")) {
                info.installName += "/" + (qtPath + "lib/").simplified();
                info.frameworkDirectory = info.installName;
                state = FrameworkName;
                continue;
            } else if (trimmed.startsWith("/") == false) {      // If the line does not contain a full path, the app is using a binary Qt package.
                if (currentPart.contains(".framework")) {
                    info.frameworkDirectory = "/Library/Frameworks/";
                    state = FrameworkName;
                } else {
                    info.frameworkDirectory = "/usr/lib/";
                    state = DylibName;
                }

                --part;
                continue;
            }
            qtPath += (currentPart + "/");

        } if (state == FrameworkName) {
            // remove ".framework"
            name = currentPart;
            name.chop(QString(".framework").length());
            info.frameworkName = currentPart;
            state = Version;
            ++part;
            continue;
        } if (state == DylibName) {
            name = currentPart.split(" (compatibility").at(0);
            info.frameworkName = name;
            info.binaryName = name.left(name.indexOf('.')) + suffix + name.mid(name.indexOf('.'));
            info.installName += name;
            info.deployedInstallName = "@executable_path/../Frameworks/" + info.binaryName;
            info.frameworkPath = info.frameworkDirectory + info.binaryName;
            info.sourceFilePath = info.frameworkPath;
            info.destinationDirectory = bundleFrameworkDirectory + "/";
            info.binaryDirectory = info.frameworkDirectory;
            info.binaryPath = info.frameworkPath;
            state = End;
            ++part;
            continue;
        } else if (state == Version) {
            info.version = currentPart;
            info.binaryDirectory = "Versions/" + info.version;
            info.binaryName = name + suffix;
            info.binaryPath = "/" + info.binaryDirectory + "/" + info.binaryName;
            info.installName += info.frameworkName + "/" + info.binaryDirectory + "/" + name;
            info.deployedInstallName = "@executable_path/../Frameworks/" + info.frameworkName + info.binaryPath;
            info.frameworkPath = info.frameworkDirectory + info.frameworkName;
            info.sourceFilePath = info.frameworkPath + info.binaryPath;
            info.destinationDirectory = bundleFrameworkDirectory + "/" + info.frameworkName + "/" + info.binaryDirectory;
            state = End;
        } else if (state == End) {
            break;
        }
    }

    return info;
}

QString findAppBinary(const QString &appBundlePath)
{
    QString appName = QFileInfo(appBundlePath).completeBaseName();
    QString binaryPath = appBundlePath  + "/Contents/MacOS/" + appName;

    if (QFile::exists(binaryPath))
        return binaryPath;
    LogError() << "Could not find bundle binary for" << appBundlePath;
    return QString();
}

QStringList findAppLibraries(const QString &appBundlePath)
{
    QStringList result;
    // dylibs
    QDirIterator iter(appBundlePath, QStringList() << QString::fromLatin1("*.dylib"),
            QDir::Files, QDirIterator::Subdirectories);

    while (iter.hasNext()) {
        iter.next();
        result << iter.fileInfo().filePath();
    }
    return result;
}


QList<FrameworkInfo> getQtFrameworks(const QStringList &otoolLines, bool useDebugLibs)
{
    QList<FrameworkInfo> libraries;
    foreach(const QString line, otoolLines) {
        FrameworkInfo info = parseOtoolLibraryLine(line, useDebugLibs);
        if (info.frameworkName.isEmpty() == false) {
            LogDebug() << "Adding framework:";
            LogDebug() << info;
            libraries.append(info);
        }
    }
    return libraries;
}

QList<FrameworkInfo> getQtFrameworks(const QString &path, bool useDebugLibs)
{
    LogDebug() << "Using otool:";
    LogDebug() << " inspecting" << path;
    QProcess otool;
    otool.start("otool", QStringList() << "-L" << path);
    otool.waitForFinished();

    if (otool.exitCode() != 0) {
        LogError() << otool.readAllStandardError();
    }

    QString output = otool.readAllStandardOutput();
    QStringList outputLines = output.split("\n");
    outputLines.removeFirst(); // remove line containing the binary path
    if (path.contains(".framework") || path.contains(".dylib"))
        outputLines.removeFirst(); // frameworks and dylibs print install name of themselves first.

    return getQtFrameworks(outputLines, useDebugLibs);
}

QList<FrameworkInfo> getQtFrameworksForPaths(const QStringList &paths, bool useDebugLibs)
{
    QList<FrameworkInfo> result;
    QSet<QString> existing;
    foreach (const QString &path, paths) {
        foreach (const FrameworkInfo &info, getQtFrameworks(path, useDebugLibs)) {
            if (!existing.contains(info.frameworkPath)) { // avoid duplicates
                existing.insert(info.frameworkPath);
                result << info;
            }
        }
    }
    return result;
}

// copies everything _inside_ sourcePath to destinationPath
void recursiveCopy(const QString &sourcePath, const QString &destinationPath)
{
    QDir().mkpath(destinationPath);

    QStringList files = QDir(sourcePath).entryList(QStringList() << "*", QDir::Files | QDir::NoDotAndDotDot);
    foreach (QString file, files) {
        const QString fileSourcePath = sourcePath + "/" + file;
        const QString fileDestinationPath = destinationPath + "/" + file;
        copyFilePrintStatus(fileSourcePath, fileDestinationPath);
    }

    QStringList subdirs = QDir(sourcePath).entryList(QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QString dir, subdirs) {
        recursiveCopy(sourcePath + "/" + dir, destinationPath + "/" + dir);
    }
}

QString copyFramework(const FrameworkInfo &framework, const QString path)
{
    QString from = framework.sourceFilePath;

    if (!QFile::exists(from)) {
        LogError() << "no file at" << from;
        return QString();
    }

    QFileInfo fromDirInfo(framework.frameworkPath + QLatin1Char('/')
                      + framework.binaryDirectory);
    bool fromDirIsSymLink = fromDirInfo.isSymLink();
    QString unresolvedToDir = path + "/" + framework.destinationDirectory;
    QString resolvedToDir;
    QString relativeLinkTarget; // will contain the link from Current to e.g. 4 in the Versions directory
    if (fromDirIsSymLink) {
        // handle the case where framework is referenced with Versions/Current
        // which is a symbolic link, so copy to target and recreate as symbolic link
        relativeLinkTarget = QDir(fromDirInfo.canonicalPath())
                .relativeFilePath(QFileInfo(fromDirInfo.symLinkTarget()).canonicalFilePath());
        resolvedToDir = QFileInfo(unresolvedToDir).path() + QLatin1Char('/') + relativeLinkTarget;
    } else {
        resolvedToDir = unresolvedToDir;
    }

    QString to = resolvedToDir + "/" + framework.binaryName;

    // create the (non-symlink) dir
    QDir dir;
    if (!dir.mkpath(resolvedToDir)) {
        LogError() << "could not create destination directory" << to;
        return QString();
    }

    if (!QFile::exists(to)) { // copy the binary and resources if that wasn't done before
        copyFilePrintStatus(from, to);

        const QString resourcesSourcePath = framework.frameworkPath + "/Resources";
        const QString resourcesDestianationPath = path + "/Contents/Frameworks/" + framework.frameworkName + "/Resources";
        recursiveCopy(resourcesSourcePath, resourcesDestianationPath);
    }

    // create the Versions/Current symlink dir if necessary
    if (fromDirIsSymLink) {
        QFile::link(relativeLinkTarget, unresolvedToDir);
        LogNormal() << " linked:" << unresolvedToDir;
        LogNormal() << " to" << resolvedToDir << "(" << relativeLinkTarget << ")";
    }
    return to;
}

void runInstallNameTool(QStringList options)
{
    QProcess installNametool;
    installNametool.start("install_name_tool", options);
    installNametool.waitForFinished();
    if (installNametool.exitCode() != 0) {
        LogError() << installNametool.readAllStandardError();
        LogError() << installNametool.readAllStandardOutput();
    }
}

void changeIdentification(const QString &id, const QString &binaryPath)
{
    LogDebug() << "Using install_name_tool:";
    LogDebug() << " change identification in" << binaryPath;
    LogDebug() << " to" << id;
    runInstallNameTool(QStringList() << "-id" << id << binaryPath);
}

void changeInstallName(const QString &bundlePath, const FrameworkInfo &framework, const QStringList &binaryPaths, bool useLoaderPath)
{
    const QString absBundlePath = QFileInfo(bundlePath).absoluteFilePath();
    foreach (const QString &binary, binaryPaths) {
        QString deployedInstallName;
        if (useLoaderPath) {
            deployedInstallName = QLatin1String("@loader_path/")
                    + QFileInfo(binary).absoluteDir().relativeFilePath(absBundlePath + QLatin1Char('/') + framework.destinationDirectory + QLatin1Char('/') + framework.binaryName);
        } else {
            deployedInstallName = framework.deployedInstallName;
        }
        changeInstallName(framework.installName, deployedInstallName, binary);
    }
}

void changeInstallName(const QString &oldName, const QString &newName, const QString &binaryPath)
{
    LogDebug() << "Using install_name_tool:";
    LogDebug() << " in" << binaryPath;
    LogDebug() << " change reference" << oldName;
    LogDebug() << " to" << newName;
    runInstallNameTool(QStringList() << "-change" << oldName << newName << binaryPath);
}

void runStrip(const QString &binaryPath)
{
    if (runStripEnabled == false)
        return;

    LogDebug() << "Using strip:";
    LogDebug() << " stripped" << binaryPath;
    QProcess strip;
    strip.start("strip", QStringList() << "-x" << binaryPath);
    strip.waitForFinished();
    if (strip.exitCode() != 0) {
        LogError() << strip.readAllStandardError();
        LogError() << strip.readAllStandardOutput();
    }
}

/*
    Deploys the the listed frameworks listed into an app bundle.
    The frameworks are searched for dependencies, which are also deployed.
    (deploying Qt3Support will also deploy QtNetwork and QtSql for example.)
    Returns a DeploymentInfo structure containing the Qt path used and a
    a list of actually deployed frameworks.
*/
DeploymentInfo deployQtFrameworks(QList<FrameworkInfo> frameworks,
        const QString &bundlePath, const QStringList &binaryPaths, bool useDebugLibs,
                                  bool useLoaderPath)
{
    LogNormal();
    LogNormal() << "Deploying Qt frameworks found inside:" << binaryPaths;
    QStringList copiedFrameworks;
    DeploymentInfo deploymentInfo;
    deploymentInfo.useLoaderPath = useLoaderPath;

    while (frameworks.isEmpty() == false) {
        const FrameworkInfo framework = frameworks.takeFirst();
        copiedFrameworks.append(framework.frameworkName);

        // Get the qt path from one of the Qt frameworks;
        if (deploymentInfo.qtPath.isNull() && framework.frameworkName.contains("Qt")
            && framework.frameworkDirectory.contains("/lib"))
        {
            deploymentInfo.qtPath = framework.frameworkDirectory;
            deploymentInfo.qtPath.chop(5); // remove "/lib/"
        }

        if (framework.installName.startsWith("/@executable_path/")) {
            LogError()  << framework.frameworkName << "already deployed, skipping.";
            continue;
        }

        // Install_name_tool the new id into the binaries
        changeInstallName(bundlePath, framework, binaryPaths, useLoaderPath);

        // Copy farmework to app bundle.
        const QString deployedBinaryPath = copyFramework(framework, bundlePath);
        // Skip the rest if already was deployed.
        if (deployedBinaryPath.isNull())
            continue;

        runStrip(deployedBinaryPath);

        // Install_name_tool it a new id.
        changeIdentification(framework.deployedInstallName, deployedBinaryPath);
        // Check for framework dependencies
        QList<FrameworkInfo> dependencies = getQtFrameworks(deployedBinaryPath, useDebugLibs);

        foreach (FrameworkInfo dependency, dependencies) {
            changeInstallName(bundlePath, dependency, QStringList() << deployedBinaryPath, useLoaderPath);

            // Deploy framework if necessary.
            if (copiedFrameworks.contains(dependency.frameworkName) == false && frameworks.contains(dependency) == false) {
                frameworks.append(dependency);
            }
        }
    }
    deploymentInfo.deployedFrameworks = copiedFrameworks;
    return deploymentInfo;
}

DeploymentInfo deployQtFrameworks(const QString &appBundlePath, const QStringList &additionalExecutables, bool useDebugLibs)
{
   ApplicationBundleInfo applicationBundle;
   applicationBundle.path = appBundlePath;
   applicationBundle.binaryPath = findAppBinary(appBundlePath);
   applicationBundle.libraryPaths = findAppLibraries(appBundlePath);
   QStringList allBinaryPaths = QStringList() << applicationBundle.binaryPath << applicationBundle.libraryPaths
                                                 << additionalExecutables;
   QList<FrameworkInfo> frameworks = getQtFrameworksForPaths(allBinaryPaths, useDebugLibs);
   if (frameworks.isEmpty()) {
        LogWarning();
        LogWarning() << "Could not find any external Qt frameworks to deploy in" << appBundlePath;
        LogWarning() << "Perhaps macdeployqt was already used on" << appBundlePath << "?";
        LogWarning() << "If so, you will need to rebuild" << appBundlePath << "before trying again.";
        return DeploymentInfo();
   } else {
       return deployQtFrameworks(frameworks, applicationBundle.path, allBinaryPaths, useDebugLibs, !additionalExecutables.isEmpty());
   }
}

void deployPlugins(const ApplicationBundleInfo &appBundleInfo, const QString &pluginSourcePath,
        const QString pluginDestinationPath, DeploymentInfo deploymentInfo, bool useDebugLibs)
{
    LogNormal() << "Deploying plugins from" << pluginSourcePath;
    QStringList plugins = QDir(pluginSourcePath).entryList(QStringList() << "*.dylib");

    foreach (QString pluginName, plugins) {
        if (pluginSourcePath.contains(deploymentInfo.pluginPath)) {
            QStringList deployedFrameworks = deploymentInfo.deployedFrameworks;

            // Skip the debug versions of the plugins, unless specified otherwise.
            if (!useDebugLibs && pluginName.endsWith("_debug.dylib"))
                continue;

            // Skip the release versions of the plugins, unless specified otherwise.
            if (useDebugLibs && !pluginName.endsWith("_debug.dylib"))
                continue;

            // Skip the designer plugins
            if (pluginSourcePath.contains("plugins/designer"))
                continue;

#ifndef QT_GRAPHICSSYSTEM_OPENGL
            // SKip the opengl graphicssystem plugin when not in use.
            if (pluginName.contains("libqglgraphicssystem"))
                continue;
#endif
            // Deploy accessibility for Qt3Support only if the Qt3Support.framework is in use
            if (deployedFrameworks.indexOf("Qt3Support.framework") == -1 && pluginName.contains("accessiblecompatwidgets"))
                continue;

            // Deploy the svg icon plugin if QtSvg.framework is in use.
            if (deployedFrameworks.indexOf("QtSvg.framework") == -1 && pluginName.contains("svg"))
                continue;

            // Deploy the phonon plugins if phonon.framework is in use
            if (deployedFrameworks.indexOf("phonon.framework") == -1 && pluginName.contains("phonon"))
                continue;

            // Deploy the sql plugins if QtSql.framework is in use
            if (deployedFrameworks.indexOf("QtSql.framework") == -1 && pluginName.contains("sql"))
                continue;

            // Deploy the script plugins if QtScript.framework is in use
            if (deployedFrameworks.indexOf("QtScript.framework") == -1 && pluginName.contains("script"))
                continue;
        }

        QDir dir;
        dir.mkpath(pluginDestinationPath);

        const QString sourcePath = pluginSourcePath + "/" + pluginName;
        const QString destinationPath = pluginDestinationPath + "/" + pluginName;
        if (copyFilePrintStatus(sourcePath, destinationPath)) {

            runStrip(destinationPath);

            // Special case for the phonon plugin: CoreVideo is not available as a separate framework
            // on panther, link against the QuartzCore framework instead. (QuartzCore contians CoreVideo.)
            if (pluginName.contains("libphonon_qt7")) {
                changeInstallName("/System/Library/Frameworks/CoreVideo.framework/Versions/A/CoreVideo",
                        "/System/Library/Frameworks/QuartzCore.framework/Versions/A/QuartzCore",
                        destinationPath);
            }

            QList<FrameworkInfo> frameworks = getQtFrameworks(destinationPath, useDebugLibs);
            deployQtFrameworks(frameworks, appBundleInfo.path, QStringList() << destinationPath, useDebugLibs, deploymentInfo.useLoaderPath);
        }
    } // foreach plugins

    QStringList subdirs = QDir(pluginSourcePath).entryList(QStringList() << "*", QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (const QString &subdir, subdirs)
        deployPlugins(appBundleInfo, pluginSourcePath + "/" + subdir, pluginDestinationPath + "/" + subdir, deploymentInfo, useDebugLibs);
}

void createQtConf(const QString &appBundlePath)
{
    QByteArray contents = "[Paths]\nPlugins = PlugIns\n";
    QString filePath = appBundlePath + "/Contents/Resources/";
    QString fileName = filePath + "qt.conf";

    QDir().mkpath(filePath);

    QFile qtconf(fileName);
    if (qtconf.exists()) {
        LogWarning();
        LogWarning() << fileName << "already exists, will not overwrite.";
        LogWarning() << "To make sure the plugins are loaded from the correct location,";
        LogWarning() << "please make sure qt.conf contains the following lines:";
        LogWarning() << "[Paths]";
        LogWarning() << "  Plugins = PlugIns";
        return;
    }

    qtconf.open(QIODevice::WriteOnly);
    if (qtconf.write(contents) != -1) {
        LogNormal() << "Created configuration file:" << fileName;
        LogNormal() << "This file sets the plugin search path to" << appBundlePath + "/Contents/PlugIns";
    }
}

void deployPlugins(const QString &appBundlePath, DeploymentInfo deploymentInfo, bool useDebugLibs)
{
    ApplicationBundleInfo applicationBundle;
    applicationBundle.path = appBundlePath;
    applicationBundle.binaryPath = findAppBinary(appBundlePath);

    const QString pluginDestinationPath = appBundlePath + "/" + "Contents/PlugIns";
    deployPlugins(applicationBundle, deploymentInfo.pluginPath, pluginDestinationPath, deploymentInfo, useDebugLibs);
}


void changeQtFrameworks(const QList<FrameworkInfo> frameworks, const QStringList &binaryPaths, const QString &absoluteQtPath)
{
    LogNormal() << "Changing" << binaryPaths << "to link against";
    LogNormal() << "Qt in" << absoluteQtPath;
    QString finalQtPath = absoluteQtPath;

    if (!absoluteQtPath.startsWith("/Library/Frameworks"))
        finalQtPath += "/lib/";

    foreach (FrameworkInfo framework, frameworks) {
        const QString oldBinaryId = framework.installName;
        const QString newBinaryId = finalQtPath + framework.frameworkName +  framework.binaryPath;
        foreach (const QString &binary, binaryPaths)
            changeInstallName(oldBinaryId, newBinaryId, binary);
    }
}

void changeQtFrameworks(const QString appPath, const QString &qtPath, bool useDebugLibs)
{
    const QString appBinaryPath = findAppBinary(appPath);
    const QStringList libraryPaths = findAppLibraries(appPath);
    const QList<FrameworkInfo> frameworks = getQtFrameworksForPaths(QStringList() << appBinaryPath << libraryPaths, useDebugLibs);
    if (frameworks.isEmpty()) {
        LogWarning();
        LogWarning() << "Could not find any _external_ Qt frameworks to change in" << appPath;
        return;
    } else {
        const QString absoluteQtPath = QDir(qtPath).absolutePath();
        changeQtFrameworks(frameworks, QStringList() << appBinaryPath << libraryPaths, absoluteQtPath);
    }
}


void createDiskImage(const QString &appBundlePath)
{
    QString appBaseName = appBundlePath;
    appBaseName.chop(4); // remove ".app" from end

    QString dmgName = appBaseName + ".dmg";

    QFile dmg(dmgName);

    if (dmg.exists()) {
        LogNormal() << "Disk image already exists, skipping .dmg creation for" << dmg.fileName();
    } else {
        LogNormal() << "Creating disk image (.dmg) for" << appBundlePath;
    }

    // More dmg options can be found in the hdiutil man page.
    QStringList options = QStringList()
            << "create" << dmgName
            << "-srcfolder" << appBundlePath
            << "-format" << "UDZO"
            << "-volname" << appBaseName;

    QProcess hdutil;
    hdutil.start("hdiutil", options);
    hdutil.waitForFinished(-1);
}
