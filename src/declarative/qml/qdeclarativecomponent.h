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

#ifndef QDECLARATIVECOMPONENT_H
#define QDECLARATIVECOMPONENT_H

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativeerror.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeCompiledData;
class QByteArray;
class QDeclarativeComponentPrivate;
class QDeclarativeEngine;
class QDeclarativeComponentAttached;
class Q_DECLARATIVE_EXPORT QDeclarativeComponent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeComponent)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl url READ url CONSTANT)

public:
    QDeclarativeComponent(QObject *parent = 0);
    QDeclarativeComponent(QDeclarativeEngine *, QObject *parent=0);
    QDeclarativeComponent(QDeclarativeEngine *, const QString &fileName, QObject *parent = 0);
    QDeclarativeComponent(QDeclarativeEngine *, const QUrl &url, QObject *parent = 0);
    virtual ~QDeclarativeComponent();

    Q_ENUMS(Status)
    enum Status { Null, Ready, Loading, Error };
    Status status() const;

    bool isNull() const;
    bool isReady() const;
    bool isError() const;
    bool isLoading() const;

    QList<QDeclarativeError> errors() const;
    Q_INVOKABLE QString errorString() const;

    qreal progress() const;

    QUrl url() const;

    virtual QObject *create(QDeclarativeContext *context = 0);
    virtual QObject *beginCreate(QDeclarativeContext *);
    virtual void completeCreate();

    void loadUrl(const QUrl &url);
    void setData(const QByteArray &, const QUrl &baseUrl);

    QDeclarativeContext *creationContext() const;

    static QDeclarativeComponentAttached *qmlAttachedProperties(QObject *);

Q_SIGNALS:
    void statusChanged(QDeclarativeComponent::Status);
    void progressChanged(qreal);

protected:
    QDeclarativeComponent(QDeclarativeComponentPrivate &dd, QObject* parent);
    Q_INVOKABLE QScriptValue createObject(QObject* parent);

private:
    QDeclarativeComponent(QDeclarativeEngine *, QDeclarativeCompiledData *, int, int, QObject *parent);

    Q_DISABLE_COPY(QDeclarativeComponent)
    friend class QDeclarativeVME;
    friend class QDeclarativeCompositeTypeData;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeComponent::Status)
QML_DECLARE_TYPE(QDeclarativeComponent)
QML_DECLARE_TYPEINFO(QDeclarativeComponent, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // QDECLARATIVECOMPONENT_H
