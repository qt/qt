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

#ifndef QMLCOMPONENT_H
#define QMLCOMPONENT_H

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtScript/qscriptvalue.h>
#include <QtDeclarative/qfxglobal.h>
#include <QtDeclarative/qml.h>
#include <QtDeclarative/qmlerror.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QmlCompiledData;
class QByteArray;
class QmlComponentPrivate;
class QmlEngine;
class Q_DECLARATIVE_EXPORT QmlComponent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QmlComponent)

public:
    QmlComponent(QObject *parent = 0);
    QmlComponent(QmlEngine *, QObject *parent=0);
    QmlComponent(QmlEngine *, const QString &url, QObject *parent = 0);
    QmlComponent(QmlEngine *, const QUrl &url, QObject *parent = 0);
    QmlComponent(QmlEngine *, const QByteArray &data, 
                 const QUrl &baseUrl, QObject *parent=0);
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

    QUrl url() const;

    virtual QObject *create(QmlContext *context = 0);
    virtual QObject *beginCreate(QmlContext *);
    virtual void completeCreate();

    void loadUrl(const QUrl &url);
    void setData(const QByteArray &, const QUrl &baseUrl);

Q_SIGNALS:
    void statusChanged(QmlComponent::Status);

protected:
    QmlComponent(QmlComponentPrivate &dd, QObject* parent);

private:
    QmlComponent(QmlEngine *, QmlCompiledData *, int, int, QObject *parent);

    friend class QmlVME;
    friend struct QmlCompositeTypeData;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QmlComponent::Status)
QML_DECLARE_TYPE(QmlComponent)

QT_END_HEADER

#endif // QMLCOMPONENT_H
