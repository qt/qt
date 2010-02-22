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
class QmlCompositeTypeData;

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

    QVariant contextProperty(const QString &) const;

    QUrl resolvedUrl(const QUrl &);

    void setBaseUrl(const QUrl &);
    QUrl baseUrl() const;

private:
    friend class QmlVME;
    friend class QmlEngine;
    friend class QmlEnginePrivate;
    friend class QmlExpression;
    friend class QmlExpressionPrivate;
    friend class QmlContextScriptClass;
    friend class QmlObjectScriptClass;
    friend class QmlComponent;
    friend class QmlComponentPrivate;
    friend class QmlScriptPrivate;
    friend class QmlBoundSignalProxy;
    QmlContext(QmlContext *parent, QObject *objParent, bool);
    QmlContext(QmlEngine *, bool);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLCONTEXT_H
