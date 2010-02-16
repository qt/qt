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

#include "qmlbinding.h"
#include "qmlbinding_p.h"

#include "qml.h"
#include "qmlcontext.h"
#include "qmlinfo.h"
#include "qmlcontext_p.h"
#include "qmldeclarativedata_p.h"
#include "qmlstringconverters_p.h"

#include <qfxperf_p_p.h>

#include <QVariant>
#include <QtCore/qdebug.h>

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

QML_DEFINE_NOCREATE_TYPE(QmlBinding);

QmlBindingData::QmlBindingData()
: updating(false), enabled(false)
{
}

QmlBindingData::~QmlBindingData()
{
    removeError();
}

void QmlBindingData::refresh()
{
    if (enabled && !updating && q) {
        QmlBinding *b = static_cast<QmlBinding *>(QmlExpressionPrivate::get(q));
        b->update();
    }
}

QmlBindingPrivate::QmlBindingPrivate()
: QmlExpressionPrivate(new QmlBindingData)
{
}

QmlBinding::QmlBinding(void *data, QmlRefCount *rc, QObject *obj, QmlContext *ctxt, const QString &url, int lineNumber, QObject *parent)
: QmlExpression(ctxt, data, rc, obj, url, lineNumber, *new QmlBindingPrivate)
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
    QmlPerfTimer<QmlPerf::BindableValueUpdate> bu;
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
                       !data->property.write(value, flags)) {

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
                QmlEnginePrivate *p = (data->context() && data->context()->engine())?
                    QmlEnginePrivate::get(data->context()->engine()):0;
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

void QmlBinding::emitValueChanged()
{
    update();
    // don't bother calling valueChanged()
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

void QmlAbstractBinding::destroy()
{
    delete this;
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

void QmlAbstractBinding::clear()
{
    if (m_mePtr)
        *m_mePtr = 0;
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
