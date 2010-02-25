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

#include "qdeclarativebinding_p.h"
#include "qdeclarativebinding_p_p.h"

#include "qdeclarative.h"
#include "qdeclarativecontext.h"
#include "qdeclarativeinfo.h"
#include "qdeclarativecontext_p.h"
#include "qdeclarativedeclarativedata_p.h"
#include "qdeclarativestringconverters_p.h"

#include <qfxperf_p_p.h>

#include <QVariant>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

QDeclarativeBindingData::QDeclarativeBindingData()
: updating(false), enabled(false)
{
}

QDeclarativeBindingData::~QDeclarativeBindingData()
{
    removeError();
}

void QDeclarativeBindingData::refresh()
{
    if (enabled && !updating && q) {
        QDeclarativeBinding *b = static_cast<QDeclarativeBinding *>(QDeclarativeExpressionPrivate::get(q));
        b->update();
    }
}

QDeclarativeBindingPrivate::QDeclarativeBindingPrivate()
: QDeclarativeExpressionPrivate(new QDeclarativeBindingData)
{
}

QDeclarativeBinding::QDeclarativeBinding(void *data, QDeclarativeRefCount *rc, QObject *obj, QDeclarativeContext *ctxt, const QString &url, int lineNumber, QObject *parent)
: QDeclarativeExpression(ctxt, data, rc, obj, url, lineNumber, *new QDeclarativeBindingPrivate)
{
    setParent(parent);
    setNotifyOnValueChanged(true);
}

QDeclarativeBinding::QDeclarativeBinding(const QString &str, QObject *obj, QDeclarativeContext *ctxt, QObject *parent)
: QDeclarativeExpression(ctxt, str, obj, *new QDeclarativeBindingPrivate)
{
    setParent(parent);
    setNotifyOnValueChanged(true);
}

QDeclarativeBinding::~QDeclarativeBinding()
{
}

void QDeclarativeBinding::setTarget(const QDeclarativeMetaProperty &prop)
{
    Q_D(QDeclarativeBinding);
    d->bindingData()->property = prop;

    update();
}

QDeclarativeMetaProperty QDeclarativeBinding::property() const 
{
   Q_D(const QDeclarativeBinding);
   return d->bindingData()->property; 
}

void QDeclarativeBinding::update(QDeclarativeMetaPropertyPrivate::WriteFlags flags)
{
    Q_D(QDeclarativeBinding);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QDeclarativePerfTimer<QDeclarativePerf::BindableValueUpdate> bu;
#endif
    QDeclarativeBindingData *data = d->bindingData();

    if (!data->enabled)
        return;

    data->addref();

    if (!data->updating) {
        data->updating = true;

        if (data->property.propertyType() == qMetaTypeId<QDeclarativeBinding *>()) {

            int idx = data->property.coreIndex();
            Q_ASSERT(idx != -1);


            QDeclarativeBinding *t = this;
            int status = -1;
            void *a[] = { &t, 0, &status, &flags };
            QMetaObject::metacall(data->property.object(),
                                  QMetaObject::WriteProperty,
                                  idx, a);

        } else {
            bool isUndefined = false;
            QVariant value = this->value(&isUndefined);

            if (isUndefined && !data->error.isValid() && data->property.isResettable()) {

                data->property.reset();

            } else if (isUndefined && !data->error.isValid()) {

                QUrl url = QUrl(data->url);
                int line = data->line;
                if (url.isEmpty()) url = QUrl(QLatin1String("<Unknown File>"));

                data->error.setUrl(url);
                data->error.setLine(line);
                data->error.setColumn(-1);
                data->error.setDescription(QLatin1String("Unable to assign [undefined] to ") + QLatin1String(QMetaType::typeName(data->property.propertyType())));

            } else if (!isUndefined && data->property.object() && 
                       !QDeclarativeMetaPropertyPrivate::write(data->property, value, flags)) {

                QUrl url = QUrl(data->url);
                int line = data->line;
                if (url.isEmpty()) url = QUrl(QLatin1String("<Unknown File>"));

                const char *valueType = 0;
                if (value.userType() == QVariant::Invalid) valueType = "null";
                else valueType = QMetaType::typeName(value.userType());

                data->error.setUrl(url);
                data->error.setLine(line);
                data->error.setColumn(-1);
                data->error.setDescription(QLatin1String("Unable to assign ") +
                                           QLatin1String(valueType) +
                                           QLatin1String(" to ") +
                                           QLatin1String(QMetaType::typeName(data->property.propertyType())));
            }

            if (data->error.isValid()) {
                QDeclarativeEnginePrivate *p = (data->context() && data->context()->engine())?
                    QDeclarativeEnginePrivate::get(data->context()->engine()):0;
               if (!data->addError(p)) 
                   qWarning().nospace() << qPrintable(this->error().toString());
            } else {
                data->removeError();
            }
        }

        data->updating = false;
    } else {
        qmlInfo(data->property.object()) << tr("Binding loop detected for property \"%1\"").arg(data->property.name());
    }

    data->release();
}

void QDeclarativeBindingPrivate::emitValueChanged()
{
    Q_Q(QDeclarativeBinding);
    q->update();
}

void QDeclarativeBinding::setEnabled(bool e, QDeclarativeMetaPropertyPrivate::WriteFlags flags)
{
    Q_D(QDeclarativeBinding);
    d->bindingData()->enabled = e;
    setNotifyOnValueChanged(e);

    QDeclarativeAbstractBinding::setEnabled(e, flags);

    if (e) {
        addToObject(d->bindingData()->property.object());
        update(flags);
    } else {
        removeFromObject();
    }
}

int QDeclarativeBinding::propertyIndex()
{
    Q_D(QDeclarativeBinding);
    return d->bindingData()->property.coreIndex();
}

bool QDeclarativeBinding::enabled() const
{
    Q_D(const QDeclarativeBinding);

    return d->bindingData()->enabled;
}

QString QDeclarativeBinding::expression() const
{
    return QDeclarativeExpression::expression();
}

QDeclarativeAbstractBinding::QDeclarativeAbstractBinding()
: m_object(0), m_mePtr(0), m_prevBinding(0), m_nextBinding(0)
{
}

QDeclarativeAbstractBinding::~QDeclarativeAbstractBinding()
{
    removeFromObject();
    if (m_mePtr)
        *m_mePtr = 0;
}

void QDeclarativeAbstractBinding::destroy()
{
    delete this;
}

void QDeclarativeAbstractBinding::addToObject(QObject *object)
{
    Q_ASSERT(object);

    removeFromObject();

    Q_ASSERT(!m_prevBinding);

    QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(object, true);
    m_nextBinding = data->bindings;
    if (m_nextBinding) m_nextBinding->m_prevBinding = &m_nextBinding;
    m_prevBinding = &data->bindings;
    data->bindings = this;
    m_object = object;

    data->setBindingBit(m_object, propertyIndex());
}

void QDeclarativeAbstractBinding::removeFromObject()
{
    if (m_prevBinding) {
        Q_ASSERT(m_object);

        *m_prevBinding = m_nextBinding;
        if (m_nextBinding) m_nextBinding->m_prevBinding = m_prevBinding;
        m_prevBinding = 0;
        m_nextBinding = 0;

        QDeclarativeDeclarativeData *data = QDeclarativeDeclarativeData::get(m_object, false);
        if (data) data->clearBindingBit(propertyIndex());
        m_object = 0;
    }
}

void QDeclarativeAbstractBinding::clear()
{
    if (m_mePtr)
        *m_mePtr = 0;
}

QString QDeclarativeAbstractBinding::expression() const
{
    return QLatin1String("<Unknown>");
}

void QDeclarativeAbstractBinding::setEnabled(bool e, QDeclarativeMetaPropertyPrivate::WriteFlags)
{
    if (e) m_mePtr = 0;
}

QT_END_NAMESPACE
