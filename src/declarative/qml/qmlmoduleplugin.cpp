/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qmlmoduleplugin.h"
#include "qstringlist.h"

QT_BEGIN_NAMESPACE

/*!
    \class QmlModulePlugin
    \brief The QmlModulePlugin class provides an abstract base for custom QML module plugins.
    \reentrant
    \ingroup plugins

    The QML module plugin is a simple plugin interface that makes it
    easy to add custom QML modules that can be loaded dynamically
    into applications.

    Writing a QML module plugin is achieved by subclassing this base
    class, reimplementing the pure virtual function keys(), and
    exporting the class with the Q_EXPORT_PLUGIN2() macro. See \l{How
    to Create Qt Plugins} for details.

    The strings returned by keys() should be the list of URIs of modules
    that the plugin registers.

    The plugin should register QML types with qmlRegisterType() when the
    defineModule() method is called.

    \sa examples/declarative/plugins
*/

/*!
    Constructs a QML module plugin with the given \a parent. This is
    invoked automatically by the Q_EXPORT_PLUGIN2() macro.
*/
QmlModulePlugin::QmlModulePlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroys the QML module plugin.

    You never have to call this explicitly. Qt destroys a plugin
    automatically when it is no longer used.
*/
QmlModulePlugin::~QmlModulePlugin()
{
}

/*!
    \fn void QmlModulePlugin::defineModule(const QString& uri)

    Subclasses must override this function to register types
    of the module \a uri, which will be one of the strings returned by keys().

    The plugin registers QML types with qmlRegisterType():

    \code
        qmlRegisterType<MyClass>("com.nokia.MyModule", 1, 0, "MyType", "MyClass");
    \endcode
*/

void QmlModulePlugin::defineModuleOnce(const QString& uri)
{ 
    if (!defined.contains(uri)) {
        defined += uri;
        defineModule(uri);
    }
}

QT_END_NAMESPACE
