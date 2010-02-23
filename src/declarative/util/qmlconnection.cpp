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

#include "qmlconnection_p.h"

#include <qmlexpression.h>
#include <qmlboundsignal_p.h>
#include <qmlcontext.h>

#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QmlConnectionPrivate : public QObjectPrivate
{
public:
    QmlConnectionPrivate() : boundsignal(0), signalSender(0), scriptset(false), componentcomplete(false) {}

    QmlBoundSignal *boundsignal;
    QObject *signalSender;
    QmlScriptString script;
    bool scriptset;
    QString signal;
    bool componentcomplete;
};

/*!
    \qmlclass Connection QmlConnection
  \since 4.7
    \brief A Connection object describes generalized connections to signals.

    When connecting to signals in QML, the usual way is to create an
    "on<Signal>" handler that reacts when a signal is received, like this:

    \qml
    MouseArea {
        onClicked: { foo(x+123,y+456) }
    }
    \endqml

    However, in some cases, it is not possible to connect to a signal in this
    way. For example, JavaScript-in-HTML style signal properties do not allow:

    \list
        \i connecting to signals with the same name but different parameters
        \i conformance checking that parameters are correctly named
        \i multiple connections to the same signal
        \i connections outside the scope of the signal sender
        \i signals in classes with coincidentally-named on<Signal> properties
    \endlist

    When any of these are needed, the Connection object can be used instead.

    For example, the above code can be changed to use a Connection object,
    like this:

    \qml
    MouseArea {
        Connection {
            signal: "clicked(x,y)"
            script: { foo(x+123,y+456) }
        }
    }
    \endqml

    More generally, the Connection object can be a child of some other object than
    the sender of the signal, and the script is the default attribute:

    \qml
    MouseArea {
        id: mr
    }
    ...
    Connection {
        sender: mr
        signal: "clicked(x,y)"
        script: { foo(x+123,y+456) }
    }
    \endqml
*/

/*!
    \internal
    \class QmlConnection
    \brief The QmlConnection class describes generalized connections to signals.

*/
QmlConnection::QmlConnection(QObject *parent) :
    QObject(*(new QmlConnectionPrivate), parent)
{
}

QmlConnection::~QmlConnection()
{
    Q_D(QmlConnection);
    delete d->boundsignal;
}

/*!
    \qmlproperty Object Connection::sender
    This property holds the object that sends the signal.

    By default, the sender is assumed to be the parent of the Connection.
*/
QObject *QmlConnection::signalSender() const
{
    Q_D(const QmlConnection);
    return d->signalSender ? d->signalSender : parent();
}

void QmlConnection::setSignalSender(QObject *obj)
{
    Q_D(QmlConnection);
    if (d->signalSender == obj)
        return;
    disconnectIfValid();
    d->signalSender = obj;
    connectIfValid();
}

void QmlConnection::connectIfValid()
{
    Q_D(QmlConnection);
    if (!d->componentcomplete)
        return;
    // boundsignal must not exist
    if ((d->signalSender || parent()) && !d->signal.isEmpty() && d->scriptset) {
        // create
        // XXX scope?
        int sigIdx = -1;
        int lparen = d->signal.indexOf(QLatin1Char('('));
        QList<QByteArray> sigparams;
        if (lparen >= 0 && d->signal.length() > lparen+2) {
            QStringList l = d->signal.mid(lparen+1,d->signal.length()-lparen-2).split(QLatin1Char(','));
            foreach (const QString &s, l) {
                sigparams.append(s.trimmed().toUtf8());
            }
        }
        QString signalname = d->signal.left(lparen);
        QObject *sender = d->signalSender ? d->signalSender : parent();
        const QMetaObject *mo = sender->metaObject();
        int methods = mo->methodCount();
        for (int ii = 0; ii < methods; ++ii) {
            QMetaMethod method = mo->method(ii);
            QString methodName = QString::fromUtf8(method.signature());
            int idx = methodName.indexOf(QLatin1Char('('));
            methodName = methodName.left(idx);
            if (methodName == signalname && (lparen<0 || method.parameterNames() == sigparams)) {
                sigIdx = ii;
                break;
            }
        }
        if (sigIdx < 0) {
            // Cannot usefully warn, since could be in middle of
            // changing sender and signal.
            // XXX need state change transactions to do better
            return;
        }

        d->boundsignal = new QmlBoundSignal(qmlContext(this), d->script.script(), sender, mo->method(sigIdx), this);
    }
}

void QmlConnection::disconnectIfValid()
{
    Q_D(QmlConnection);
    if (!d->componentcomplete)
        return;
    if ((d->signalSender || parent()) && !d->signal.isEmpty() && d->scriptset) {
        // boundsignal must exist
        // destroy
        delete d->boundsignal;
        d->boundsignal = 0;
    }
}

void QmlConnection::componentComplete()
{
    Q_D(QmlConnection);
    d->componentcomplete=true;
    connectIfValid();
}


/*!
    \qmlproperty script Connection::script
    This property holds the JavaScript executed whenever the signal is sent.

    This is the default attribute of Connection.
*/
QmlScriptString QmlConnection::script() const
{
    Q_D(const QmlConnection);
    return d->script;
}

void QmlConnection::setScript(const QmlScriptString& script)
{
    Q_D(QmlConnection);
    if ((d->signalSender || parent()) && !d->signal.isEmpty()) {
        if (!d->scriptset) {
            // mustn't exist - create
            d->scriptset = true;
            d->script = script;
            connectIfValid();
        } else {
            // must exist - update
            d->script = script;
            d->boundsignal->expression()->setExpression(script.script());
        }
    } else {
        d->scriptset = true;
        d->script = script;
    }
}

/*!
    \qmlproperty string Connection::signal
    This property holds the signal from the sender to which the script is attached.

    The signal's formal parameter names must be given in parentheses:

    \qml
Connection {
    signal: "clicked(x,y)"
    script: { ... }
}
    \endqml
*/
QString QmlConnection::signal() const
{
    Q_D(const QmlConnection);
    return d->signal;
}

void QmlConnection::setSignal(const QString& sig)
{
    Q_D(QmlConnection);
    if (d->signal == sig)
        return;
    disconnectIfValid();
    d->signal =  sig;
    connectIfValid();
}

QML_DEFINE_TYPE(Qt,4,6,Connection,QmlConnection)

QT_END_NAMESPACE
