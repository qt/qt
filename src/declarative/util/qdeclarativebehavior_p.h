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

#ifndef QDECLARATIVEBEHAVIOR_H
#define QDECLARATIVEBEHAVIOR_H

#include "private/qdeclarativestate_p.h"

#include <qdeclarativepropertyvaluesource.h>
#include <qdeclarativepropertyvalueinterceptor.h>
#include <qdeclarative.h>
#include <QtCore/QAbstractAnimation>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeAbstractAnimation;
class QDeclarativeBehaviorPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeBehavior : public QObject, public QDeclarativePropertyValueInterceptor
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeBehavior)

    Q_INTERFACES(QDeclarativePropertyValueInterceptor)
    Q_CLASSINFO("DefaultProperty", "animation")
    Q_PROPERTY(QDeclarativeAbstractAnimation *animation READ animation WRITE setAnimation)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_CLASSINFO("DeferredPropertyNames", "animation")

public:
    QDeclarativeBehavior(QObject *parent=0);
    ~QDeclarativeBehavior();

    virtual void setTarget(const QDeclarativeProperty &);
    virtual void write(const QVariant &value);

    QDeclarativeAbstractAnimation *animation();
    void setAnimation(QDeclarativeAbstractAnimation *);

    bool enabled() const;
    void setEnabled(bool enabled);

Q_SIGNALS:
    void enabledChanged();

private Q_SLOTS:
    void componentFinalized();
    void qtAnimationStateChanged(QAbstractAnimation::State,QAbstractAnimation::State);
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeBehavior)

QT_END_HEADER

#endif // QDECLARATIVEBEHAVIOR_H
