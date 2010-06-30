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

#include "qdeclarativenetworkaccessmanagerfactory.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDeclarativeNetworkAccessManagerFactory
    \since 4.7
    \brief The QDeclarativeNetworkAccessManagerFactory class provides a factory for QNetworkAccessManager for use by a Qt Declarative engine.

    QNetworkAccessManager is used for all network access by QML.
    By implementing a factory it is possible to create custom
    QNetworkAccessManager with specialized caching, proxy and
    cookie support.

    To implement a factory, subclass QDeclarativeNetworkAccessManagerFactory and implement
    the create() method.

    To use a factory, assign it to the relevant QDeclarativeEngine using
    QDeclarativeEngine::setNetworkAccessManagerFactory().

    Note: the create() method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.

    \sa QDeclarativeEngine::setNetworkAccessManagerFactory(), {declarative/cppextensions/networkaccessmanagerfactory}{NetworkAccessManagerFactory example}
*/

/*!
  The destructor is empty.
 */
QDeclarativeNetworkAccessManagerFactory::~QDeclarativeNetworkAccessManagerFactory()
{
}

/*!
    \fn QNetworkAccessManager *QDeclarativeNetworkAccessManagerFactory::create(QObject *parent)

    Implement this method to create a QNetworkAccessManager with \a parent.
    This allows proxies, caching and cookie support to be setup appropriately.

    This method must return a new QNetworkAccessManager each time it is called.
    The parent of the QNetworkAccessManager must be the \a parent provided.
    The QNetworkAccessManager(s) created by this
    function will be destroyed automatically when their parent is destroyed.

    Note: this method may be called by multiple threads, so ensure the
    implementation of this method is reentrant.
*/

QT_END_NAMESPACE
