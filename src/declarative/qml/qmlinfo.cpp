/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmlinfo.h"
#include <private/qmldeclarativedata_p.h>
#include <QtDeclarative/qmlcontext.h>

QT_BEGIN_NAMESPACE

/*!
    \class QmlInfo
    \brief The QmlInfo class prints warnings messages that include the file and line number for QML types.

    When QML types display warning messages, it improves tracibility
    if they include the QML file and line number on which the
    particular instance was instantiated.
    
    QmlInfo statements work just like regular Qt qDebug() statements.
    To include the file and line number, an object must be passed.  If
    the file and line number is not available for that instance
    (either it was not instantiated by the QML engine or location
    information is disabled), "unknown location" will be used instead.

    For example, 

    \code
    qmlInfo(this) << "component property is a write-once property";
    \endcode

    prints

    \code
    QML ComponentInstance (unknown location): component property is a write-once property
    \endcode
*/

/*!
    Construct a QmlInfo, using \a object for file and line number
    information.
*/
QmlInfo::QmlInfo(QObject *object)
: QDebug(QtWarningMsg)
{
    *this << "QML";
    if (object) 
        *this << object->metaObject()->className();
    QmlDeclarativeData *ddata = QmlDeclarativeData::get(object);
    if (ddata) {
        QString location = QLatin1String("(");
        if (ddata->outerContext) {
            location += ddata->outerContext->baseUrl().toString();
        } else {
            location += "unknown";
        }
        location += QLatin1String(":");
        location += QString::number(ddata->lineNumber);
        location += QLatin1String(":");
        location += QString::number(ddata->columnNumber);
        location += QLatin1String(")");
        *this << location.toLatin1().constData();
    } else {
        *this << "(unknown location):";
    }
}

/*!
  The destructor does nothing special.
*/
QmlInfo::~QmlInfo()
{
}

/*!
  \relates QmlInfo
  \fn QmlInfo qmlInfo(QObject *me)
  Constructs an instance of QmlInfo from \a me and returns it.
*/

QT_END_NAMESPACE
