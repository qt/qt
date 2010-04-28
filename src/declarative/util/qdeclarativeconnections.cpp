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

#include "private/qdeclarativeconnections_p.h"

#include <qdeclarativeexpression.h>
#include <qdeclarativeproperty_p.h>
#include <qdeclarativeboundsignal_p.h>
#include <qdeclarativecontext.h>
#include <qdeclarativeinfo.h>

#include <QtCore/qdebug.h>
#include <QtCore/qstringlist.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeConnectionsPrivate : public QObjectPrivate
{
public:
    QDeclarativeConnectionsPrivate() : target(0), componentcomplete(false) {}

    QList<QDeclarativeBoundSignal*> boundsignals;
    QObject *target;

    bool componentcomplete;

    QByteArray data;
};

/*!
    \qmlclass Connections QDeclarativeConnections
  \since 4.7
    \brief A Connections object describes generalized connections to signals.

    When connecting to signals in QML, the usual way is to create an
    "on<Signal>" handler that reacts when a signal is received, like this:

    \qml
    MouseArea {
        onClicked: { foo(...) }
    }
    \endqml

    However, in some cases, it is not possible to connect to a signal in this
    way, such as:

    \list
        \i multiple connections to the same signal
        \i connections outside the scope of the signal sender
        \i connections to targets not defined in QML
    \endlist

    When any of these are needed, the Connections object can be used instead.

    For example, the above code can be changed to use a Connections object,
    like this:

    \qml
    MouseArea {
        Connections {
            onClicked: foo(...)
        }
    }
    \endqml

    More generally, the Connections object can be a child of some other object than
    the sender of the signal:

    \qml
    MouseArea {
        id: area
    }
    ...
    Connections {
        target: area
        onClicked: foo(...)
    }
    \endqml

    \sa QtDeclarative
*/

/*!
    \internal
    \class QDeclarativeConnections
    \brief The QDeclarativeConnections class describes generalized connections to signals.

*/
QDeclarativeConnections::QDeclarativeConnections(QObject *parent) :
    QObject(*(new QDeclarativeConnectionsPrivate), parent)
{
}

QDeclarativeConnections::~QDeclarativeConnections()
{
}

/*!
    \qmlproperty Object Connections::target
    This property holds the object that sends the signal.

    By default, the target is assumed to be the parent of the Connections.
*/
QObject *QDeclarativeConnections::target() const
{
    Q_D(const QDeclarativeConnections);
    return d->target ? d->target : parent();
}

void QDeclarativeConnections::setTarget(QObject *obj)
{
    Q_D(QDeclarativeConnections);
    if (d->target == obj)
        return;
    foreach (QDeclarativeBoundSignal *s, d->boundsignals) {
        // It is possible that target is being changed due to one of our signal
        // handlers -> use deleteLater().
        if (s->isEvaluating())
            s->deleteLater();
        else
            delete s;
    }
    d->boundsignals.clear();
    d->target = obj;
    connectSignals();
    emit targetChanged();
}


QByteArray
QDeclarativeConnectionsParser::compile(const QList<QDeclarativeCustomParserProperty> &props)
{
    QByteArray rv;
    QDataStream ds(&rv, QIODevice::WriteOnly);

    for(int ii = 0; ii < props.count(); ++ii)
    {
        QString propName = QString::fromUtf8(props.at(ii).name());
        if (!propName.startsWith(QLatin1String("on")) || !propName.at(2).isUpper()) {
            error(props.at(ii), QDeclarativeConnections::tr("Cannot assign to non-existent property \"%1\"").arg(propName));
            return QByteArray();
        }

        QList<QVariant> values = props.at(ii).assignedValues();

        for (int i = 0; i < values.count(); ++i) {
            const QVariant &value = values.at(i);

            if (value.userType() == qMetaTypeId<QDeclarativeCustomParserNode>()) {
                error(props.at(ii), QDeclarativeConnections::tr("Connections: nested objects not allowed"));
                return QByteArray();
            } else if (value.userType() == qMetaTypeId<QDeclarativeCustomParserProperty>()) {
                error(props.at(ii), QDeclarativeConnections::tr("Connections: syntax error"));
                return QByteArray();
            } else {
                QDeclarativeParser::Variant v = qvariant_cast<QDeclarativeParser::Variant>(value);
                if (v.isScript()) {
                    ds << propName;
                    ds << v.asScript();
                } else {
                    error(props.at(ii), QDeclarativeConnections::tr("Connections: script expected"));
                    return QByteArray();
                }
            }
        }
    }

    return rv;
}

void QDeclarativeConnectionsParser::setCustomData(QObject *object,
                                            const QByteArray &data)
{
    QDeclarativeConnectionsPrivate *p =
        static_cast<QDeclarativeConnectionsPrivate *>(QObjectPrivate::get(object));
    p->data = data;
}


void QDeclarativeConnections::connectSignals()
{
    Q_D(QDeclarativeConnections);
    if (!d->componentcomplete)
        return;

    QDataStream ds(d->data);
    while (!ds.atEnd()) {
        QString propName;
        ds >> propName;
        QString script;
        ds >> script;
        QDeclarativeProperty prop(target(), propName);
        if (!prop.isValid()) {
            qmlInfo(this) << tr("Cannot assign to non-existent property \"%1\"").arg(propName);
        } else if (prop.type() & QDeclarativeProperty::SignalProperty) {
            QDeclarativeBoundSignal *signal =
                new QDeclarativeBoundSignal(target(), prop.method(), this);
            signal->setExpression(new QDeclarativeExpression(qmlContext(this), script, 0));
            d->boundsignals += signal;
        } else {
            qmlInfo(this) << tr("Cannot assign to non-existent property \"%1\"").arg(propName);
        }
    }
}

void QDeclarativeConnections::componentComplete()
{
    Q_D(QDeclarativeConnections);
    d->componentcomplete=true;
    connectSignals();
}

QT_END_NAMESPACE
