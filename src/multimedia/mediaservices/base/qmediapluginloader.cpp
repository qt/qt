/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMediaServices module of the Qt Toolkit.
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

#include "qmediapluginloader_p.h"
#include <QtCore/qcoreapplication.h>
#include <QtCore/qpluginloader.h>
#include <QtCore/qdir.h>
#include <QtCore/qdebug.h>

#include <QtMediaServices/qmediaserviceproviderplugin.h>


QT_BEGIN_NAMESPACE


typedef QMap<QString,QObjectList> ObjectListMap;
Q_GLOBAL_STATIC(ObjectListMap, staticMediaPlugins);

QMediaPluginLoader::QMediaPluginLoader(const char *iid, const QString &location, Qt::CaseSensitivity):
    m_iid(iid)
{
    m_location = location + QLatin1String("/");
    load();
}

QStringList QMediaPluginLoader::keys() const
{
    return m_instances.keys();
}

QObject* QMediaPluginLoader::instance(QString const &key)
{
    return m_instances.value(key);
}

QList<QObject*> QMediaPluginLoader::instances(QString const &key)
{
    return m_instances.values(key);
}

//to be used for testing purposes only
void QMediaPluginLoader::setStaticPlugins(const QString &location, const QObjectList& objects)
{
    staticMediaPlugins()->insert(location + QLatin1String("/"), objects);
}

void QMediaPluginLoader::load()
{
    if (!m_instances.isEmpty())
        return;

    if (staticMediaPlugins() && staticMediaPlugins()->contains(m_location)) {
        foreach(QObject *o, staticMediaPlugins()->value(m_location)) {
            if (o != 0 && o->qt_metacast(m_iid) != 0) {
                QFactoryInterface* p = qobject_cast<QFactoryInterface*>(o);
                if (p != 0) {
                    foreach (QString const &key, p->keys())
                        m_instances.insertMulti(key, o);
                }
            }
        }
    } else {
#ifndef QT_NO_LIBRARY
        QStringList     paths = QCoreApplication::libraryPaths();

        foreach (QString const &path, paths) {
            QString     pluginPathName(path + m_location);
            QDir        pluginDir(pluginPathName);

            if (!pluginDir.exists())
                continue;

            foreach (const QString &pluginLib, pluginDir.entryList(QDir::Files)) {
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
                if (pluginLib.endsWith(QLatin1String(".debug")))
                    continue;
#endif
                QPluginLoader   loader(pluginPathName + pluginLib);

                QObject *o = loader.instance();
                if (o != 0 && o->qt_metacast(m_iid) != 0) {
                    QFactoryInterface* p = qobject_cast<QFactoryInterface*>(o);
                    if (p != 0) {
                        foreach (QString const &key, p->keys())
                            m_instances.insertMulti(key, o);
                    }

                    continue;
                } else {
                    qWarning() << "QMediaPluginLoader: Failed to load plugin: " << pluginLib << loader.errorString();
                }
                delete o;
                loader.unload();
            }
        }
#endif
    }
}

QT_END_NAMESPACE

