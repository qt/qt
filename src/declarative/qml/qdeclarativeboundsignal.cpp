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

#include "private/qdeclarativeboundsignal_p.h"

#include "private/qmetaobjectbuilder_p.h"
#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativeexpression_p.h"
#include "private/qdeclarativecontext_p.h"
#include "private/qdeclarativemetatype_p.h"
#include "qdeclarative.h"
#include "qdeclarativecontext.h"
#include "private/qdeclarativeglobal_p.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

class QDeclarativeBoundSignalParameters : public QObject
{
Q_OBJECT
public:
    QDeclarativeBoundSignalParameters(const QMetaMethod &, QObject * = 0);
    ~QDeclarativeBoundSignalParameters();

    void setValues(void **);
    void clearValues();

private:
    friend class MetaObject;
    int metaCall(QMetaObject::Call, int _id, void **);
    struct MetaObject : public QAbstractDynamicMetaObject {
        MetaObject(QDeclarativeBoundSignalParameters *b)
            : parent(b) {}

        int metaCall(QMetaObject::Call c, int id, void **a) { 
            return parent->metaCall(c, id, a);
        }
        QDeclarativeBoundSignalParameters *parent;
    };

    int *types;
    void **values;
    QMetaObject *myMetaObject;
};

static int evaluateIdx = -1;

QDeclarativeAbstractBoundSignal::QDeclarativeAbstractBoundSignal(QObject *parent)
: QObject(parent)
{
}

QDeclarativeAbstractBoundSignal::~QDeclarativeAbstractBoundSignal()
{
}

QDeclarativeBoundSignal::QDeclarativeBoundSignal(QObject *scope, const QMetaMethod &signal, 
                               QObject *parent)
: m_expression(0), m_signal(signal), m_paramsValid(false), m_isEvaluating(false), m_params(0)
{
    // This is thread safe.  Although it may be updated by two threads, they
    // will both set it to the same value - so the worst thing that can happen
    // is that they both do the work to figure it out.  Boo hoo.
    if (evaluateIdx == -1) evaluateIdx = metaObject()->methodCount();

    QDeclarative_setParent_noEvent(this, parent);
    QMetaObject::connect(scope, m_signal.methodIndex(), this, evaluateIdx);
}

QDeclarativeBoundSignal::QDeclarativeBoundSignal(QDeclarativeContext *ctxt, const QString &val, 
                               QObject *scope, const QMetaMethod &signal,
                               QObject *parent)
: m_expression(0), m_signal(signal), m_paramsValid(false), m_isEvaluating(false), m_params(0)
{
    // This is thread safe.  Although it may be updated by two threads, they
    // will both set it to the same value - so the worst thing that can happen
    // is that they both do the work to figure it out.  Boo hoo.
    if (evaluateIdx == -1) evaluateIdx = metaObject()->methodCount();

    QDeclarative_setParent_noEvent(this, parent);
    QMetaObject::connect(scope, m_signal.methodIndex(), this, evaluateIdx);

    m_expression = new QDeclarativeExpression(ctxt, scope, val);
}

QDeclarativeBoundSignal::~QDeclarativeBoundSignal()
{
    delete m_expression;
    m_expression = 0;
}

int QDeclarativeBoundSignal::index() const 
{ 
    return m_signal.methodIndex();
}

/*!
    Returns the signal expression.
*/
QDeclarativeExpression *QDeclarativeBoundSignal::expression() const
{
    return m_expression;
}

/*!
    Sets the signal expression to \a e.  Returns the current signal expression,
    or null if there is no signal expression.

    The QDeclarativeBoundSignal instance takes ownership of \a e.  The caller is 
    assumes ownership of the returned QDeclarativeExpression.
*/
QDeclarativeExpression *QDeclarativeBoundSignal::setExpression(QDeclarativeExpression *e)
{
    QDeclarativeExpression *rv = m_expression;
    m_expression = e;
    if (m_expression) m_expression->setNotifyOnValueChanged(false);
    return rv;
}

QDeclarativeBoundSignal *QDeclarativeBoundSignal::cast(QObject *o)
{
    QDeclarativeAbstractBoundSignal *s = qobject_cast<QDeclarativeAbstractBoundSignal*>(o);
    return static_cast<QDeclarativeBoundSignal *>(s);
}

int QDeclarativeBoundSignal::qt_metacall(QMetaObject::Call c, int id, void **a)
{
    if (c == QMetaObject::InvokeMetaMethod && id == evaluateIdx) {
        m_isEvaluating = true;
        if (!m_paramsValid) {
            if (!m_signal.parameterTypes().isEmpty())
                m_params = new QDeclarativeBoundSignalParameters(m_signal, this);
            m_paramsValid = true;
        }

        if (m_params) m_params->setValues(a);
        if (m_expression && m_expression->engine()) {
            QDeclarativeExpressionPrivate::get(m_expression)->value(m_params);
            if (m_expression && m_expression->hasError())
                QDeclarativeEnginePrivate::warning(m_expression->engine(), m_expression->error());
        }
        if (m_params) m_params->clearValues();
        m_isEvaluating = false;
        return -1;
    } else {
        return QObject::qt_metacall(c, id, a);
    }
}

QDeclarativeBoundSignalParameters::QDeclarativeBoundSignalParameters(const QMetaMethod &method, 
                                                   QObject *parent)
: QObject(parent), types(0), values(0)
{
    MetaObject *mo = new MetaObject(this);

    // ### Optimize!
    // ### Ensure only supported types are allowed, otherwise it might crash
    QMetaObjectBuilder mob;
    mob.setSuperClass(&QDeclarativeBoundSignalParameters::staticMetaObject);
    mob.setClassName("QDeclarativeBoundSignalParameters");

    QList<QByteArray> paramTypes = method.parameterTypes();
    QList<QByteArray> paramNames = method.parameterNames();
    types = new int[paramTypes.count()];
    for (int ii = 0; ii < paramTypes.count(); ++ii) {
        const QByteArray &type = paramTypes.at(ii);
        const QByteArray &name = paramNames.at(ii);

        if (name.isEmpty() || type.isEmpty()) {
            types[ii] = 0;
            continue;
        }

        QVariant::Type t = (QVariant::Type)QMetaType::type(type.constData());
        if (QDeclarativeMetaType::isQObject(t)) {
            types[ii] = QMetaType::QObjectStar;
            QMetaPropertyBuilder prop = mob.addProperty(name, "QObject*");
            prop.setWritable(false);
        } else {
            types[ii] = t;
            QMetaPropertyBuilder prop = mob.addProperty(name, type);
            prop.setWritable(false);
        }
    }
    myMetaObject = mob.toMetaObject();
    *static_cast<QMetaObject *>(mo) = *myMetaObject;

    d_ptr->metaObject = mo;
}

QDeclarativeBoundSignalParameters::~QDeclarativeBoundSignalParameters()
{
    delete [] types;
    qFree(myMetaObject);
}

void QDeclarativeBoundSignalParameters::setValues(void **v)
{
    values = v;
}

void QDeclarativeBoundSignalParameters::clearValues()
{
    values = 0;
}

int QDeclarativeBoundSignalParameters::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (!values)
        return -1;

    if (c == QMetaObject::ReadProperty && id >= 1) {
        QDeclarativeMetaType::copy(types[id - 1], a[0], values[id]);
        return -1;
    } else {
        return qt_metacall(c, id, a);
    }
}

QT_END_NAMESPACE

#include <qdeclarativeboundsignal.moc>
