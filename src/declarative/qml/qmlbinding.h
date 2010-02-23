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

#ifndef QMLBINDING_H
#define QMLBINDING_H

#include "qml.h"
#include "qmlpropertyvaluesource.h"
#include "qmlexpression.h"

#include <QtCore/QObject>
#include <QtCore/QMetaProperty>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_DECLARATIVE_EXPORT QmlAbstractBinding
{
public:
    QmlAbstractBinding();
    virtual ~QmlAbstractBinding();

    virtual void destroy();

    virtual QString expression() const;

    void setEnabled(bool e) { setEnabled(e, QmlMetaProperty::DontRemoveBinding); }
    virtual void setEnabled(bool, QmlMetaProperty::WriteFlags) = 0;
    virtual int propertyIndex() = 0;

    void update() { update(QmlMetaProperty::DontRemoveBinding); }
    virtual void update(QmlMetaProperty::WriteFlags) = 0;

    void addToObject(QObject *);
    void removeFromObject();

protected:
    void clear();

private:
    friend class QmlDeclarativeData;
    friend class QmlMetaProperty;
    friend class QmlMetaPropertyPrivate;
    friend class QmlVME;

    QObject *m_object;
    QmlAbstractBinding **m_mePtr;
    QmlAbstractBinding **m_prevBinding;
    QmlAbstractBinding  *m_nextBinding;
};

class QmlContext;
class QmlBindingPrivate;
class Q_DECLARATIVE_EXPORT QmlBinding : public QmlExpression, 
                                        public QmlAbstractBinding
{
Q_OBJECT
public:
    QmlBinding(const QString &, QObject *, QmlContext *, QObject *parent=0);
    QmlBinding(void *, QmlRefCount *, QObject *, QmlContext *, const QString &, int, 
               QObject *parent);
    ~QmlBinding();

    void setTarget(const QmlMetaProperty &);
    QmlMetaProperty property() const;

    bool enabled() const;

    // Inherited from  QmlAbstractBinding
    virtual void setEnabled(bool, QmlMetaProperty::WriteFlags flags);
    virtual int propertyIndex();
    virtual void update(QmlMetaProperty::WriteFlags flags);
    virtual QString expression() const;

public Q_SLOTS:
    void update() { update(QmlMetaProperty::DontRemoveBinding); }

protected:
    void emitValueChanged();

private:
    Q_DECLARE_PRIVATE(QmlBinding)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QmlBinding);

QT_END_HEADER

#endif // QMLBINDING_H
