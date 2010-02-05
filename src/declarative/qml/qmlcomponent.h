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

#ifndef QMLCOMPONENT_H
#define QMLCOMPONENT_H

#include "qml.h"
#include "qmlerror.h"

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlCompiledData;
class QByteArray;
class QmlComponentPrivate;
class QmlEngine;
class QmlComponentAttached;
class Q_DECLARATIVE_EXPORT QmlComponent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlComponent)
    Q_PROPERTY(bool isNull READ isNull NOTIFY statusChanged)
    Q_PROPERTY(bool isReady READ isReady NOTIFY statusChanged)
    Q_PROPERTY(bool isError READ isError NOTIFY statusChanged)
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl url READ url CONSTANT)

public:
    QmlComponent(QObject *parent = 0);
    QmlComponent(QmlEngine *, QObject *parent=0);
    QmlComponent(QmlEngine *, const QString &fileName, QObject *parent = 0);
    QmlComponent(QmlEngine *, const QUrl &url, QObject *parent = 0);
    virtual ~QmlComponent();

    Q_ENUMS(Status)
    enum Status { Null, Ready, Loading, Error };
    Status status() const;

    bool isNull() const;
    bool isReady() const;
    bool isError() const;
    bool isLoading() const;

    QList<QmlError> errors() const;
    Q_INVOKABLE QString errorsString() const;

    qreal progress() const;

    QUrl url() const;

    virtual QObject *create(QmlContext *context = 0);
    virtual QObject *beginCreate(QmlContext *);
    virtual void completeCreate();

    Q_INVOKABLE QScriptValue createObject();

    void loadUrl(const QUrl &url);
    void setData(const QByteArray &, const QUrl &baseUrl);

    void setCreationContext(QmlContext*);
    QmlContext *creationContext() const;

    static QmlComponentAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void statusChanged(QmlComponent::Status);
    void progressChanged(qreal);

protected:
    QmlComponent(QmlComponentPrivate &dd, QObject* parent);

private:
    QmlComponent(QmlEngine *, QmlCompiledData *, int, int, QObject *parent);

    friend class QmlVME;
    friend class QmlCompositeTypeData;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QmlComponent::Status)
QML_DECLARE_TYPE(QmlComponent)
QML_DECLARE_TYPEINFO(QmlComponent, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // QMLCOMPONENT_H
