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

#ifndef QMLCONTEXT_H
#define QMLCONTEXT_H

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QmlEngine;
class QmlRefCount;
class QmlContextPrivate;
class Q_DECLARATIVE_EXPORT QmlContext : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlContext)

public:
    QmlContext(QmlEngine *parent, QObject *objParent=0);
    QmlContext(QmlContext *parent, QObject *objParent=0);
    virtual ~QmlContext();

    QmlEngine *engine() const;
    QmlContext *parentContext() const;

    void addDefaultObject(QObject *);
    void setContextProperty(const QString &, QObject *);
    void setContextProperty(const QString &, const QVariant &);

    void activate();
    void deactivate();

    static QmlContext *activeContext();

    QUrl resolvedUri(const QUrl &);
    QUrl resolvedUrl(const QUrl &);

    void setBaseUrl(const QUrl &);

private Q_SLOTS:
    void objectDestroyed(QObject *);

private:
    friend class QmlVME;
    friend class QmlEngine;
    friend class QmlEnginePrivate;
    friend class QmlExpression;
    friend class QmlExpressionPrivate;
    friend class QmlBasicScript;
    friend class QmlContextScriptClass;
    friend class QmlObjectScriptClass;
    friend class QmlComponent;
    friend class QmlScriptPrivate;
    friend class QmlBoundSignalProxy;
    friend class QmlSimpleDeclarativeData;
    QmlContext(QmlContext *parent, QObject *objParent, bool);
    QmlContext(QmlEngine *, bool);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLCONTEXT_H
