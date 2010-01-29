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

#include "qmlnetworkaccessmanagerfactory.h"

QT_BEGIN_NAMESPACE

/*!
    \class QmlNetworkAccessManagerFactory
    \brief The QmlNetworkAccessManagerFactory class provides a factory for QNetworkAccessManager

    QNetworkAccessManager is used for all network access by QML.
    By implementing a factory it is possible to create custom
    QNetworkAccessManager with specialized caching, proxy and
    cookie support.

    To implement a factory, subclass QmlNetworkAccessManagerFactory and implement
    the create() method.

    If the created QNetworkAccessManager becomes invalid, due to a
    change in proxy settings, for example, call the invalidate() method.
    This will cause all QNetworkAccessManagers to be recreated.

    Note: the create() method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.
*/
QmlNetworkAccessManagerFactory::~QmlNetworkAccessManagerFactory()
{
}

/*!
    \fn QNetworkAccessManager *QmlNetworkAccessManagerFactory::create(QObject *parent)

    Implement this method to create a QNetworkAccessManager with \a parent.
    This allows proxies, caching and cookie support to be setup appropriately.

    Note: this method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.
*/

/*!
    Invalidates all currently created QNetworkAccessManager(s) which
    will cause create() to be called for subsequent network access.
*/
void QmlNetworkAccessManagerFactory::invalidate()
{
    emit invalidated();
}

/*!
    \internal
    \fn QmlNetworkAccessManagerFactory::invalidated()
*/

QT_END_NAMESPACE
