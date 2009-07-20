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

Q_DECLARE_METATYPE(QList<QObject *>);

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(scriptWarnings, QML_SCRIPT_WARNINGS);

QmlBindingPrivate::QmlBindingPrivate()
: inited(false), updating(false), enabled(true), mePtr(0)
{
}

QML_DEFINE_NOCREATE_TYPE(QmlBinding);
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
    Q_D(QmlBinding);
    if(d->mePtr)
        *(d->mePtr) = 0;
}

void QmlBinding::setTarget(const QmlMetaProperty &prop)
{
    Q_D(QmlBinding);
    d->property = prop;

    update();
}

QmlMetaProperty QmlBinding::property() const 
{
   Q_D(const QmlBinding);
   return d->property; 
}

void QmlBinding::init()
{
    Q_D(QmlBinding);

    if (d->inited)
        return;
    d->inited = true;
    update();
}

void QmlBinding::setExpression(const QString &expr)
{
    QmlExpression::setExpression(expr);
    update();
}

void QmlBinding::forceUpdate()
{
    Q_D(QmlBinding);
    if (!d->inited)
        init();
    else
        update();
}

void QmlBinding::update()
{
    Q_D(QmlBinding);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindableValueUpdate> bu;
#endif
    if (!d->inited || !d->enabled)
        return;

    if (!d->updating) {
        d->updating = true;

        if (d->property.propertyCategory() == QmlMetaProperty::Bindable) {

            int idx = d->property.coreIndex();
            Q_ASSERT(idx != -1);

            void *a[1];
            QmlBinding *t = this;
            a[0] = (void *)&t;
            QMetaObject::metacall(d->property.object(), 
                                  QMetaObject::WriteProperty,
                                  idx, a);

        } else {

            QVariant value = this->value();
            if ((uint)d->property.propertyType() >= QVariant::UserType &&
                value.type() == QVariant::String) {
                QmlMetaType::StringConverter con = QmlMetaType::customStringConverter(d->property.propertyType());
                if (con)
                    value = con(value.toString());
            }

            if (d->property.propertyType() == QVariant::Url && 
                (value.type() == QVariant::String || value.type() == QVariant::ByteArray) && !value.isNull()) 
                value.setValue(context()->resolvedUrl(QUrl(value.toString())));

            d->property.write(value);
        }

        d->updating = false;
    } else {
        qmlInfo(d->property.object()) << "Binding loop detected for property" << d->property.name();
    }
}

void QmlBinding::valueChanged()
{
    update();
}

void QmlBinding::setEnabled(bool e)
{
    Q_D(QmlBinding);
    d->enabled = e;
    setTrackChange(e);
}

bool QmlBinding::enabled() const
{
    Q_D(const QmlBinding);

    return d->enabled;
}

QT_END_NAMESPACE
