/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qdeclarativeextensionplugin.h"

QT_BEGIN_NAMESPACE

/*!
    \since 4.7
    \class QDeclarativeExtensionPlugin
    \brief The QDeclarativeExtensionPlugin class provides an abstract base for custom QML extension plugins.

    \ingroup plugins

    QDeclarativeExtensionPlugin is a plugin interface that makes it
    possible to offer extensions that can be loaded dynamically into
    applications using the QDeclarativeEngine class.

    Writing a QML extension plugin is achieved by subclassing this
    base class, reimplementing the pure virtual registerTypes()
    function, and exporting the class using the Q_EXPORT_PLUGIN2()
    macro.

    See \l {Extending QML in C++} for details how to write a QML extension plugin.
    See \l {How to Create Qt Plugins} for general Qt plugin documentation.

    \sa QDeclarativeEngine::importPlugin()
*/

/*!
    \fn void QDeclarativeExtensionPlugin::registerTypes(const char *uri)

    Registers the QML types in the given \a uri. Here you call qmlRegisterType() for
    all types which are provided by the extension plugin.
*/

/*!
    Constructs a QML extension plugin with the given \a parent.

    Note that this constructor is invoked automatically by the
    Q_EXPORT_PLUGIN2() macro, so there is no need for calling it
    explicitly.
*/
QDeclarativeExtensionPlugin::QDeclarativeExtensionPlugin(QObject *parent)
    : QObject(parent)
{
}

/*!
  Destructor.
 */
QDeclarativeExtensionPlugin::~QDeclarativeExtensionPlugin()
{
}

/*!
    \fn void QDeclarativeExtensionPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)

    Initializes the extension from the \a uri using the \a engine.
*/

void QDeclarativeExtensionPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}

QT_END_NAMESPACE
