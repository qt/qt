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

#ifndef QDECLARATIVEBINDING_P_H
#define QDECLARATIVEBINDING_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarative.h"
#include "qdeclarativepropertyvaluesource.h"
#include "qdeclarativeexpression.h"
#include "qdeclarativeproperty.h"
#include "qdeclarativeproperty_p.h"

#include <QtCore/QObject>
#include <QtCore/QMetaProperty>

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QDeclarativeAbstractBinding
{
public:
    QDeclarativeAbstractBinding();
    virtual ~QDeclarativeAbstractBinding();

    virtual void destroy();

    virtual QString expression() const;

    void setEnabled(bool e) { setEnabled(e, QDeclarativePropertyPrivate::DontRemoveBinding); }
    virtual void setEnabled(bool, QDeclarativePropertyPrivate::WriteFlags) = 0;
    virtual int propertyIndex() = 0;

    void update() { update(QDeclarativePropertyPrivate::DontRemoveBinding); }
    virtual void update(QDeclarativePropertyPrivate::WriteFlags) = 0;

    void addToObject(QObject *);
    void removeFromObject();

protected:
    void clear();

private:
    friend class QDeclarativeDeclarativeData;
    friend class QDeclarativeProperty;
    friend class QDeclarativePropertyPrivate;
    friend class QDeclarativeVME;

    QObject *m_object;
    QDeclarativeAbstractBinding **m_mePtr;
    QDeclarativeAbstractBinding **m_prevBinding;
    QDeclarativeAbstractBinding  *m_nextBinding;
};

class QDeclarativeContext;
class QDeclarativeBindingPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeBinding : public QDeclarativeExpression, public QDeclarativeAbstractBinding
{
Q_OBJECT
public:
    QDeclarativeBinding(const QString &, QObject *, QDeclarativeContext *, QObject *parent=0);
    QDeclarativeBinding(void *, QDeclarativeRefCount *, QObject *, QDeclarativeContext *, const QString &, int, 
               QObject *parent);
    ~QDeclarativeBinding();

    void setTarget(const QDeclarativeProperty &);
    QDeclarativeProperty property() const;

    bool enabled() const;

    // Inherited from  QDeclarativeAbstractBinding
    virtual void setEnabled(bool, QDeclarativePropertyPrivate::WriteFlags flags);
    virtual int propertyIndex();
    virtual void update(QDeclarativePropertyPrivate::WriteFlags flags);
    virtual QString expression() const;

public Q_SLOTS:
    void update() { update(QDeclarativePropertyPrivate::DontRemoveBinding); }

protected:
    void emitValueChanged();

private:
    Q_DECLARE_PRIVATE(QDeclarativeBinding)
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeBinding*);

#endif // QDECLARATIVEBINDING_P_H
