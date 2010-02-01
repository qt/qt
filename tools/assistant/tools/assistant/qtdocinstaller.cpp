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
#include <QtCore/QLibraryInfo>
#include <QtCore/QDateTime>
#include <QtCore/QFileSystemWatcher>
#include <QtHelp/QHelpEngineCore>
#include "../shared/collectionconfiguration.h"
#include "qtdocinstaller.h"

QT_BEGIN_NAMESPACE

QtDocInstaller::QtDocInstaller(const QString &collectionFile,
                               QFileSystemWatcher *qchWatcher)
    : m_qchWatcher(qchWatcher)
{
    m_abort = false;
    m_collectionFile = collectionFile;
}

QtDocInstaller::~QtDocInstaller()
{
    if (!isRunning())
        return;
    m_mutex.lock();
    m_abort = true;
    m_mutex.unlock();
    wait();
}

void QtDocInstaller::installDocs()
{
    start(LowPriority);
}

void QtDocInstaller::run()
{
    QHelpEngineCore *helpEngine = new QHelpEngineCore(m_collectionFile);
    helpEngine->setupData();
    bool changes = false;

    QStringList docs;
    docs << QLatin1String("assistant")
        << QLatin1String("designer")
        << QLatin1String("linguist")
        << QLatin1String("qmake")
        << QLatin1String("qt");

    foreach (const QString &doc, docs) {
        changes |= installDoc(doc, helpEngine);
        m_mutex.lock();
        if (m_abort) {
            delete helpEngine;
            m_mutex.unlock();
            return;
        }
        m_mutex.unlock();
    }
    delete helpEngine;
    emit docsInstalled(changes);
}

bool QtDocInstaller::installDoc(const QString &name, QHelpEngineCore *helpEngine)
{
    QStringList lst = CollectionConfiguration::qtDocInfo(*helpEngine, name);

    QDateTime dt;
    if (!lst.isEmpty() && !lst.first().isEmpty())
        dt = QDateTime::fromString(lst.first(), Qt::ISODate);

    QString qchFile;
    if (lst.count() == 2)
        qchFile = lst.last();

    QDir dir(QLibraryInfo::location(QLibraryInfo::DocumentationPath)
        + QDir::separator() + QLatin1String("qch"));

    const QStringList files = dir.entryList(QStringList() << QLatin1String("*.qch"));
    if (files.isEmpty()) {
        CollectionConfiguration::setQtDocInfo(*helpEngine, name,
            QStringList(QDateTime().toString(Qt::ISODate)));
        return false;
    }
    foreach (const QString &f, files) {
        if (f.startsWith(name)) {
            QFileInfo fi(dir.absolutePath() + QDir::separator() + f);
            if (dt.isValid() && fi.lastModified().toTime_t() == dt.toTime_t()
                && qchFile == fi.absoluteFilePath())
                return false;

            QString namespaceName = QHelpEngineCore::namespaceName(fi.absoluteFilePath());
            if (namespaceName.isEmpty())
                continue;

            if (helpEngine->registeredDocumentations().contains(namespaceName)) {
                const QString docFile =
                    helpEngine->documentationFileName(namespaceName);
                if (helpEngine->unregisterDocumentation(namespaceName))
                    m_qchWatcher->removePath(docFile);
            }

            if (!helpEngine->registerDocumentation(fi.absoluteFilePath())) {
                emit errorMessage(
                    tr("The file %1 could not be registered successfully!\n\nReason: %2")
                    .arg(fi.absoluteFilePath()).arg(helpEngine->error()));
            } else {
                m_qchWatcher->addPath(fi.absoluteFilePath());
            }

            Q_ASSERT(m_qchWatcher->files().count()
                     == helpEngine->registeredDocumentations().count());

            CollectionConfiguration::setQtDocInfo(*helpEngine, name,
                QStringList() << fi.lastModified().toString(Qt::ISODate)
                              << fi.absoluteFilePath());
            return true;
        }
    }
    return false;
}

QT_END_NAMESPACE
