/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtCore/QLibraryInfo>
#include <QtCore/QUrl>
#include <QtCore/QStringList>

#include <QtGui/QApplication>
#include <QtGui/QDesktopServices>

#include <QtHelp/QHelpEngineCore>

#include <QtNetwork/QLocalSocket>

#include <QtSql/QSqlDatabase>

#include "../shared/collectionconfiguration.h"
#include "mainwindow.h"
#include "cmdlineparser.h"

QT_USE_NAMESPACE

#if defined(USE_STATIC_SQLITE_PLUGIN)
  #include <QtPlugin>
  Q_IMPORT_PLUGIN(qsqlite)
#endif

namespace {

void
updateLastPagesOnUnregister(QHelpEngineCore& helpEngine, const QString& nsName)
{
    int lastPage = CollectionConfiguration::lastTabPage(helpEngine);
    QStringList currentPages = CollectionConfiguration::lastShownPages(helpEngine);
    if (!currentPages.isEmpty()) {
        QStringList zoomList = CollectionConfiguration::lastZoomFactors(helpEngine);
        while (zoomList.count() < currentPages.count())
            zoomList.append(CollectionConfiguration::DefaultZoomFactor);

        for (int i = currentPages.count(); --i >= 0;) {
            if (QUrl(currentPages.at(i)).host() == nsName) {
                zoomList.removeAt(i);
                currentPages.removeAt(i);
                lastPage = (lastPage == (i + 1)) ? 1 : lastPage;
            }
        }

        CollectionConfiguration::setLastShownPages(helpEngine, currentPages);
        CollectionConfiguration::setLastTabPage(helpEngine, lastPage);
        CollectionConfiguration::setLastZoomFactors(helpEngine, zoomList);
    }
}

bool
updateUserCollection(QHelpEngineCore& user, const QHelpEngineCore& caller)
{
    if (!CollectionConfiguration::isNewer(caller, user))
        return false;
    CollectionConfiguration::copyConfiguration(caller, user);
    return true;
}

bool
referencedHelpFilesExistAll(QHelpEngineCore& user, QStringList& nameSpaces)
{
    QFileInfo fi;
    int counter = nameSpaces.count();
    for (int i = counter; --i >= 0;) {
        const QString& nameSpace = nameSpaces.at(i);
        fi.setFile(user.documentationFileName(nameSpace));
        if (!fi.exists() || !fi.isFile()) {
            user.unregisterDocumentation(nameSpace);
            nameSpaces.removeAll(nameSpace);
        }
    }
    return (counter != nameSpaces.count()) ? false : true;
}

QString indexFilesFolder(const QString &collectionFile)
{
    QString indexFilesFolder = QLatin1String(".fulltextsearch");
    if (!collectionFile.isEmpty()) {
        QFileInfo fi(collectionFile);
        indexFilesFolder = QLatin1Char('.') +
            fi.fileName().left(fi.fileName().lastIndexOf(QLatin1String(".qhc")));
    }
    return indexFilesFolder;
}

} // Anonymous namespace.

int main(int argc, char *argv[])
{
#ifndef Q_OS_WIN 
    // First do a quick search for arguments that imply command-line mode.
    const char * cmdModeArgs[] = {
        "-help", "-register", "-unregister", "-remove-search-index"
    };
    bool useGui = true;
    for (int i = 1; i < argc; ++i) {
        for (size_t j = 0; j < sizeof cmdModeArgs/sizeof *cmdModeArgs; ++j) {
            if(strcmp(argv[i], cmdModeArgs[j]) == 0) {
                useGui = false;
                break;
            }
        }
    }
    QApplication a(argc, argv, useGui);
#else
    QApplication a(argc, argv);
#endif
    a.addLibraryPath(a.applicationDirPath() + QLatin1String("/plugins"));

    CmdLineParser cmd(a.arguments());
    CmdLineParser::Result res = cmd.parse();
    if (res == CmdLineParser::Help)
        return 0;
    else if (res == CmdLineParser::Error)
        return -1;

    QString cmdCollectionFile = cmd.collectionFile();
    if (cmd.registerRequest() != CmdLineParser::None) {
        if (cmdCollectionFile.isEmpty())
            cmdCollectionFile = MainWindow::defaultHelpCollectionFileName();
        QHelpEngineCore help(cmdCollectionFile);
        help.setupData();
        if (cmd.registerRequest() == CmdLineParser::Register) {
            if (!help.registerDocumentation(cmd.helpFile())) {
                cmd.showMessage(
                    QObject::tr("Could not register documentation file\n%1\n\nReason:\n%2")
                    .arg(cmd.helpFile()).arg(help.error()), true);
                return -1;
            } else {
                cmd.showMessage(QObject::tr("Documentation successfully registered."),
                    false);
            }
        } else {
            QString nsName = QHelpEngineCore::namespaceName(cmd.helpFile());
            if (help.unregisterDocumentation(nsName)) {
                updateLastPagesOnUnregister(help, nsName);
                cmd.showMessage(
                    QObject::tr("Documentation successfully unregistered."),
                    false);
            } else {
                cmd.showMessage(QObject::tr("Could not unregister documentation"
                    " file\n%1\n\nReason:\n%2").arg(cmd.helpFile()).
                    arg(help.error()), true);
                return -1;
            }
        }
        CollectionConfiguration::setDocUpdatePending(help, true);
        return 0;
    }

    if (cmd.removeSearchIndex()) {
        QString file = cmdCollectionFile;
        if (file.isEmpty())
            file = MainWindow::defaultHelpCollectionFileName();
        QString path = QFileInfo(file).path();
        path += QLatin1Char('/') + indexFilesFolder(file);

        QLocalSocket localSocket;
        localSocket.connectToServer(QString(QLatin1String("QtAssistant%1"))
            .arg(QLatin1String(QT_VERSION_STR)));

        QDir dir(path); // check if there is no other instance ruinning
        if (!localSocket.waitForConnected() && dir.exists()) {
            QStringList lst = dir.entryList(QDir::Files | QDir::Hidden);
            foreach (const QString &item, lst)
                dir.remove(item);
            return 0;
        } else {
            return -1;
        }
    }

    {
        QSqlDatabase db;
        QStringList sqlDrivers(db.drivers());
        if (sqlDrivers.isEmpty()
            || !sqlDrivers.contains(QLatin1String("QSQLITE"))) {
            cmd.showMessage(QObject::tr("Cannot load sqlite database driver!"),
                true);
            return -1;
        }
    }

    if (!cmdCollectionFile.isEmpty()) {
        QHelpEngineCore caller(cmdCollectionFile);
        if (!caller.setupData()) {
            cmd.showMessage(QObject::tr("The specified collection file could "
                "not be read!"), true);
            return -1;
        }

        QString fileName = QFileInfo(cmdCollectionFile).fileName();
        const QString &cacheDir = CollectionConfiguration::cacheDir(caller);
        const QString dir = !cacheDir.isEmpty()
            && CollectionConfiguration::cacheDirIsRelativeToCollection(caller)
            ? QFileInfo(cmdCollectionFile).dir().absolutePath()
                + QDir::separator() + cacheDir
            : MainWindow::collectionFileDirectory(false, cacheDir);

        bool collectionFileExists = true;
        QFileInfo fi(dir + QDir::separator() + fileName);
        if (!fi.exists()) {
            collectionFileExists = false;
            if (!caller.copyCollectionFile(fi.absoluteFilePath())) {
                cmd.showMessage(caller.error(), true);
                return -1;
            }
        }

        if (collectionFileExists) {
            QHelpEngineCore user(fi.absoluteFilePath());
            if (user.setupData()) {
                // some docs might have been un/registered
                bool docUpdate = CollectionConfiguration::docUpdatePending(caller);

                // update in case the passed collection file changed
                if (updateUserCollection(user, caller))
                    docUpdate = true;

                QStringList userDocs = user.registeredDocumentations();
                // update user collection file, docs might have been (re)moved
                if (!referencedHelpFilesExistAll(user, userDocs))
                    docUpdate = true;

                if (docUpdate) {
                    QStringList callerDocs = caller.registeredDocumentations();
                    foreach (const QString &doc, callerDocs) {
                        if (!userDocs.contains(doc)) {
                            user.registerDocumentation(
                                caller.documentationFileName(doc));
                        }
                    }

                    QLatin1String intern("com.trolltech.com.assistantinternal-");
                    foreach (const QString &doc, userDocs) {
                        if (!callerDocs.contains(doc) && !doc.startsWith(intern))
                            user.unregisterDocumentation(doc);
                    }

                    CollectionConfiguration::setDocUpdatePending(caller, false);
                }
            }
        }
        cmd.setCollectionFile(fi.absoluteFilePath());
    }

    if (!cmd.currentFilter().isEmpty()) {
        QString collectionFile;
        if (cmdCollectionFile.isEmpty()) {
            MainWindow::collectionFileDirectory(true);
            cmdCollectionFile = MainWindow::defaultHelpCollectionFileName();
        }

        QHelpEngineCore user(cmdCollectionFile);
        if (user.setupData())
            user.setCurrentFilter(cmd.currentFilter());
    }

    const QString& locale = QLocale::system().name();
    QString resourceDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

    QTranslator translator(0);
    translator.load(QLatin1String("assistant_") + locale, resourceDir);
    a.installTranslator(&translator);

    QTranslator qtTranslator(0);
    qtTranslator.load(QLatin1String("qt_") + locale, resourceDir);
    a.installTranslator(&qtTranslator);

    QTranslator qtHelpTranslator(0);
    qtHelpTranslator.load(QLatin1String("qt_help_") + locale, resourceDir);
    a.installTranslator(&qtHelpTranslator);

    MainWindow w(&cmd);
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
