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

#include <qml.h>
#include "qmlbinding.h"
#include "qmlbinding_p.h"
#include <qmlcontext.h>
#include <qmlinfo.h>
#include <QVariant>
#include <private/qfxperf_p.h>
#include <QtCore/qdebug.h>
#include <private/qmlcontext_p.h>
#include <private/qmldeclarativedata_p.h>
#include <private/qmlstringconverters_p.h>

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

QML_DEFINE_NOCREATE_TYPE(QmlBinding);

QmlBindingData::QmlBindingData()
: updating(false), enabled(false)
{
}

QmlBindingPrivate::QmlBindingPrivate()
: QmlExpressionPrivate(new QmlBindingData)
{
}

QmlBinding::QmlBinding(void *data, QmlRefCount *rc, QObject *obj, QmlContext *ctxt, QObject *parent)
: QmlExpression(ctxt, data, rc, obj, *new QmlBindingPrivate)
{
    setParent(parent);
}

QmlBinding::QmlBinding(const QString &str, QObject *obj, QmlContext *ctxt, QObject *parent)
: QmlExpression(ctxt, str, obj, *new QmlBindingPrivate)
{
    setParent(parent);
}

QmlBinding::~QmlBinding()
{
}

void QmlBinding::setTarget(const QmlMetaProperty &prop)
{
    Q_D(QmlBinding);
    d->bindingData()->property = prop;

    update();
}

QmlMetaProperty QmlBinding::property() const 
{
   Q_D(const QmlBinding);
   return d->bindingData()->property; 
}

void QmlBinding::update(QmlMetaProperty::WriteFlags flags)
{
    Q_D(QmlBinding);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindableValueUpdate> bu;
#endif
    QmlBindingData *data = d->bindingData();

    if (!data->enabled)
        return;

    data->addref();

    if (!data->updating) {
        data->updating = true;

        if (data->property.propertyCategory() == QmlMetaProperty::Bindable) {

            int idx = data->property.coreIndex();
            Q_ASSERT(idx != -1);


            QmlBinding *t = this;
            int status = -1;
            void *a[] = { &t, 0, &status, &flags };
            QMetaObject::metacall(data->property.object(),
                                  QMetaObject::WriteProperty,
                                  idx, a);

        } else {
            QVariant value = this->value();
            data->property.write(value, flags);
        }

        data->updating = false;
    } else {
        qmlInfo(data->property.object()) << "Binding loop detected for property" 
                                         << data->property.name();
    }

    data->release();
}

void QmlBinding::valueChanged()
{
    update();
}

void QmlBinding::setEnabled(bool e, QmlMetaProperty::WriteFlags flags)
{
    Q_D(QmlBinding);
    d->bindingData()->enabled = e;
    setTrackChange(e);

    QmlAbstractBinding::setEnabled(e, flags);

    if (e) {
        addToObject(d->bindingData()->property.object());
        update(flags);
    } else {
        removeFromObject();
    }
}

int QmlBinding::propertyIndex()
{
    Q_D(QmlBinding);
    return d->bindingData()->property.coreIndex();
}

bool QmlBinding::enabled() const
{
    Q_D(const QmlBinding);

    return d->bindingData()->enabled;
}

QString QmlBinding::expression() const
{
    return QmlExpression::expression();
}

QmlAbstractBinding::QmlAbstractBinding()
: m_object(0), m_mePtr(0), m_prevBinding(0), m_nextBinding(0)
{
}

QmlAbstractBinding::~QmlAbstractBinding()
{
    removeFromObject();
    if (m_mePtr)
        *m_mePtr = 0;
}

void QmlAbstractBinding::addToObject(QObject *object)
{
    Q_ASSERT(object);

    removeFromObject();

    Q_ASSERT(!m_prevBinding);

    QmlDeclarativeData *data = QmlDeclarativeData::get(object, true);
    m_nextBinding = data->bindings;
    if (m_nextBinding) m_nextBinding->m_prevBinding = &m_nextBinding;
    m_prevBinding = &data->bindings;
    data->bindings = this;
    m_object = object;

    data->setBindingBit(m_object, propertyIndex());
}

void QmlAbstractBinding::removeFromObject()
{
    if (m_prevBinding) {
        Q_ASSERT(m_object);

        *m_prevBinding = m_nextBinding;
        if (m_nextBinding) m_nextBinding->m_prevBinding = m_prevBinding;
        m_prevBinding = 0;
        m_nextBinding = 0;

        QmlDeclarativeData *data = QmlDeclarativeData::get(m_object, false);
        if (data) data->clearBindingBit(propertyIndex());
        m_object = 0;
    }
}

QString QmlAbstractBinding::expression() const
{
    return QLatin1String("<Unknown>");
}

void QmlAbstractBinding::setEnabled(bool e, QmlMetaProperty::WriteFlags)
{
    if (e) m_mePtr = 0;
}

QT_END_NAMESPACE
