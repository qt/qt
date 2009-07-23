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

#include "qmlbindablecomponent.h"
#include "qmlbindablecomponent_p.h"
#include "qmlcomponent.h"

QT_BEGIN_NAMESPACE
QmlBindableComponent::QmlBindableComponent(QmlEngine *engine, QObject *parent)
    : QmlComponent(*(new QmlBindableComponentPrivate), parent)
{
    Q_D(QmlBindableComponent);
    d->engine = engine;
    connect(this, SIGNAL(statusChanged(QmlComponent::Status)),
            this, SLOT(statusChange(QmlComponent::Status)));
}

QmlBindableComponent::QmlBindableComponent(QmlEngine *engine, const QUrl &url, QObject *parent)
    : QmlComponent(*(new QmlBindableComponentPrivate), parent)
{
    Q_D(QmlBindableComponent);
    d->engine = engine;
    loadUrl(url);
    connect(this, SIGNAL(statusChanged(QmlComponent::Status)),
            this, SLOT(statusChange(QmlComponent::Status)));
}

void QmlBindableComponent::setContext(QmlContext* c)
{
    Q_D(QmlBindableComponent);
    d->ctxt =c;
}
/*!
    Create a script object instance from this component. Returns a null
    script object if creation failed. It will create the instance in the
    same context that it was created it. QmlBindableComponent is only
    meant to be created from with script - C++ developers should just use
    QmlComponent directly.

    Similar to QmlComponent::create(), but creates an object suitable
    for usage within scripts.
*/
QScriptValue QmlBindableComponent::createObject()
{
    Q_D(QmlBindableComponent);
    QObject* ret = create(d->ctxt);
    return QmlEngine::qmlScriptObject(ret, d->engine);
}

/*!
    Return the list of errors that occured during the last compile or create
    operation, as a single string.  An empty string is returned if isError()
    is not set.

    This function is similar to errors(), except more useful when called from
    QML. C++ code should usually use errors().

    \sa errors()
*/
QString QmlBindableComponent::errorsString() const
{
    Q_D(const QmlBindableComponent);
    QString ret;
    if(!isError())
        return ret;
    foreach(const QmlError &e, d->errors) {
        ret += e.url().toString() + QLatin1String(":") + 
               QString::number(e.line()) + QLatin1String(" ") + 
               e.description() + QLatin1String("\n");
    }
    return ret;
}


void QmlBindableComponent::statusChange(QmlComponent::Status newStatus)
{
    Q_D(QmlBindableComponent);
    if(newStatus == d->prevStatus)
        return;
    if(newStatus == QmlComponent::Null || d->prevStatus == QmlComponent::Null)
        emit isNullChanged();
    if(newStatus == QmlComponent::Ready || d->prevStatus == QmlComponent::Ready)
        emit isReadyChanged();
    if(newStatus == QmlComponent::Loading || d->prevStatus == QmlComponent::Loading)
        emit isLoadingChanged();
    if(newStatus == QmlComponent::Error || d->prevStatus == QmlComponent::Error)
        emit isErrorChanged();
    d->prevStatus = newStatus;
}

QT_END_NAMESPACE
