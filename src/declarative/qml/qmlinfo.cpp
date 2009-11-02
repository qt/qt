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

#include "qmlinfo.h"
#include <private/qmldeclarativedata_p.h>
#include <QtDeclarative/qmlcontext.h>
#include <QtGui/qapplication.h>

QT_BEGIN_NAMESPACE

/*!
    \fn void qmlInfo(const QString& message, QObject *object)

    \brief Prints warnings messages that include the file and line number for QML types.

    When QML types display warning messages, it improves tracibility
    if they include the QML file and line number on which the
    particular instance was instantiated.
    
    To include the file and line number, an object must be passed.  If
    the file and line number is not available for that instance
    (either it was not instantiated by the QML engine or location
    information is disabled), "unknown location" will be used instead.

    For example, 

    \code
    qmlInfo(tr("component property is a write-once property"), object);
    \endcode

    prints

    \code
    QML ComponentInstance (unknown location): component property is a write-once property
    \endcode
*/

void qmlInfo(const QString& msg, QObject* object)
{
    QString pos = QLatin1String("QML");
    if (object) {
        pos += QLatin1Char(' ');
        pos += QLatin1String(object->metaObject()->className());
    }
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(object);
    pos += QLatin1String(" (");
    if (ddata) {
        if (ddata->outerContext) {
            pos += ddata->outerContext->baseUrl().toString();
        } else {
            pos += qApp->translate("QmlInfo","unknown");
        }
        pos += QLatin1String(":");
        pos += QString::number(ddata->lineNumber);
        pos += QLatin1String(":");
        pos += QString::number(ddata->columnNumber);
    } else {
        pos += qApp->translate("QmlInfo","unknown location");
    }
    pos += QLatin1String(") ");
    qWarning((pos + msg).toLocal8Bit()); // XXX allow other processing?
}

QT_END_NAMESPACE
