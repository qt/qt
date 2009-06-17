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

#include "qmlconnection.h"
#include <QtDeclarative/qmlexpression.h>
#include "private/qmlboundsignal_p.h"
#include "private/qobject_p.h"
#include <QtDeclarative/qmlcontext.h>
#include <QtCore/qdebug.h>


QT_BEGIN_NAMESPACE
class QmlConnectionPrivate : public QObjectPrivate
{
public:
    QmlConnectionPrivate() : boundsignal(0), signalSender(0), componentcomplete(false) {}

    QmlBoundSignal *boundsignal;
    QObject *signalSender;
    QString script;
    QString signal;
    bool componentcomplete;
};

/*!
    \qmlclass Connection QmlConnection
    \brief A Connection object describes generalized connections to signals.

    JavaScript-in-HTML style signal properties do not allow:
    \list
        \i connecting to signals with the same name but different parameters
        \i conformance checking that parameters are correctly named
        \i multiple connections to the same signal
        \i connections outside the scope of the signal sender
        \i signals in classes with coincidentally-named on<Signal> properties
    \endlist

    When any of these is needed, the Connection object can be used instead.
    Where a signal could be connected like this:

    \qml
MouseRegion {
    onClicked: { foo(x+123,y+456) }
}
    \endqml

    An equivalent binding can be made with a Connection object:

    \qml
MouseRegion {
    Connection {
        signal: "clicked(x,y)"
        script: { foo(x+123,y+456) }
    }
}
    \endqml

    More generally, the Connection object can be a child of some other object than
    the sender of the signal, and the script is the default attribute:

    \qml
MouseRegion {
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

    QmlSetProperties is a mechanism for connecting a script to be run when
    some object sends a signal.
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

/*!
    \property QmlConnection::sender
    \brief the object that sends the signal.

    By default, the sender is assumed to be the parent of the Connection.

    Note that the set/get methods are setSignalSender() and signalSender(),
    due to the pre-existence of QObject::sender().
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
    if ((d->signalSender || parent()) && !d->signal.isEmpty() && !d->script.isEmpty()) {
        // create
        // XXX scope?
        int sigIdx = -1;
        int lparen = d->signal.indexOf(QLatin1Char('('));
        QList<QByteArray> sigparams;
        if (lparen >= 0 && d->signal.length() > lparen+2) {
            QStringList l = d->signal.mid(lparen+1,d->signal.length()-lparen-2).split(QLatin1Char(','));
            foreach (const QString &s, l) {
                sigparams.append(s.toLatin1());
            }
        }
        QString signalname = d->signal.left(lparen);
        QObject *sender = d->signalSender ? d->signalSender : parent();
        const QMetaObject *mo = sender->metaObject();
        int methods = mo->methodCount();
        for (int ii = 0; ii < methods; ++ii) {
            QMetaMethod method = mo->method(ii);
            QString methodName = QLatin1String(method.signature());
            int idx = methodName.indexOf(QLatin1Char('('));
            methodName = methodName.left(idx);
            if (methodName == signalname && (lparen<0 || method.parameterNames() == sigparams)) {
                sigIdx = ii;
                break;
            }
        }
        if (sigIdx < 0) {
            qWarning() << "signal" << d->signal << "not found";
            return;
        }

        if (sigparams.isEmpty())
            d->boundsignal = new QmlBoundSignal(qmlContext(this), d->script, sender, sigIdx, this);
        else
            d->boundsignal = new QmlBoundSignalProxy(new QmlContext(qmlContext(this),this), d->script, sender, sigIdx, this);
    }
}

void QmlConnection::disconnectIfValid()
{
    Q_D(QmlConnection);
    if (!d->componentcomplete)
        return;
    if ((d->signalSender || parent()) && !d->signal.isEmpty() && !d->script.isEmpty()) {
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
    \qmlproperty string Connection::script
    This property holds the JavaScript executed whenever the signal is sent.

    This is the default attribute of Connection.
*/

/*!
    \property QmlConnection::script
    \brief the JavaScript executed whenever the signal is sent.
*/
QString QmlConnection::script() const
{
    Q_D(const QmlConnection);
    return d->script;
}

void QmlConnection::setScript(const QString& script)
{
    Q_D(QmlConnection);
    if ((d->signalSender || parent()) && !d->signal.isEmpty()) {
        if (d->script.isEmpty()) {
            // mustn't exist - create
            d->script = script;
            connectIfValid();
        } else {
            // must exist - update
            d->script = script;
            d->boundsignal->setExpression(script);
        }
    } else {
        d->script = script;
    }
}

/*!
    \qmlproperty string Connection::signal
    This property holds the signal from the sender to which the script is attached.

    The signal must have its formal parameter names given in parentheses:

    \qml
Connection {
    signal: "clicked(x,y)"
    script: { ... }
}
    \endqml
*/

/*!
    \property QmlConnection::signal
    \brief the signal from the sender to which the script is attached.
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

QML_DEFINE_TYPE(QmlConnection,Connection)

QT_END_NAMESPACE
