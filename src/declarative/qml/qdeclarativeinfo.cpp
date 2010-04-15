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

#include "qdeclarativeinfo.h"

#include "private/qdeclarativedata_p.h"
#include "qdeclarativecontext.h"
#include "private/qdeclarativecontext_p.h"
#include "private/qdeclarativemetatype_p.h"

#include <QCoreApplication>

QT_BEGIN_NAMESPACE

/*!
    \fn QDeclarativeInfo qmlInfo(const QObject *object)

    \brief Prints warnings messages that include the file and line number for QML types.

    When QML types display warning messages, it improves traceability
    if they include the QML file and line number on which the
    particular instance was instantiated.
    
    To include the file and line number, an object must be passed.  If
    the file and line number is not available for that instance
    (either it was not instantiated by the QML engine or location
    information is disabled), "unknown location" will be used instead.

    For example, 

    \code
    qmlInfo(object) << tr("component property is a write-once property");
    \endcode

    prints

    \code
    QML MyCustomType (unknown location): component property is a write-once property
    \endcode
*/

QDeclarativeInfo::QDeclarativeInfo(const QObject *object)
: QDebug(QtWarningMsg)
{
    QString pos = QLatin1String("QML");
    if (object) {
        pos += QLatin1Char(' ');

        QString typeName;
        QDeclarativeType *type = QDeclarativeMetaType::qmlType(object->metaObject());
        if (type) {
            typeName = QLatin1String(type->qmlTypeName());
            int lastSlash = typeName.lastIndexOf(QLatin1Char('/'));
            if (lastSlash != -1)
                typeName = typeName.mid(lastSlash+1);
        } else {
            typeName = QString::fromUtf8(object->metaObject()->className());
            int marker = typeName.indexOf(QLatin1String("_QMLTYPE_"));
            if (marker != -1)
                typeName = typeName.left(marker);
        }

        pos += typeName;
    }
    QDeclarativeData *ddata = object?QDeclarativeData::get(object):0;
    pos += QLatin1String(" (");
    if (ddata) {
        if (ddata->outerContext && !ddata->outerContext->url.isEmpty()) {
            pos += ddata->outerContext->url.toString();
            pos += QLatin1Char(':');
            pos += QString::number(ddata->lineNumber);
            pos += QLatin1Char(':');
            pos += QString::number(ddata->columnNumber);
        } else {
            pos += QCoreApplication::translate("QDeclarativeInfo","unknown location");
        }
    } else {
        pos += QCoreApplication::translate("QDeclarativeInfo","unknown location");
    }
    pos += QLatin1Char(')');
    *this << pos;
    nospace();
}

QDeclarativeInfo::~QDeclarativeInfo()
{
}


QT_END_NAMESPACE
