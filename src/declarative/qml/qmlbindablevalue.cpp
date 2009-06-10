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
#include "qmlbindablevalue.h"
#include "qmlbindablevalue_p.h"
#include <qmlcontext.h>
#include <qmlinfo.h>
#include <QVariant>
#include <qfxperf.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

DEFINE_BOOL_CONFIG_OPTION(scriptWarnings, QML_SCRIPT_WARNINGS);

QmlBindableValuePrivate::QmlBindableValuePrivate()
: inited(false), updating(false), mePtr(0)
{
}

QML_DEFINE_NOCREATE_TYPE(QmlBindableValue);
QmlBindableValue::QmlBindableValue(QObject *parent)
: QmlPropertyValueSource(*new QmlBindableValuePrivate, parent)
{
    qFatal("QmlBindableValue: Default constructor not supported");
}

QmlBindableValue::QmlBindableValue(void *data, QmlRefCount *rc, QObject *obj, QObject *parent)
: QmlPropertyValueSource(*new QmlBindableValuePrivate, parent), QmlExpression(QmlContext::activeContext(), data, rc, obj)
{
}

QmlBindableValue::QmlBindableValue(const QString &str, QObject *obj, QObject *parent)
: QmlPropertyValueSource(*new QmlBindableValuePrivate, parent), QmlExpression(QmlContext::activeContext(), str, obj)
{
}

QmlBindableValue::~QmlBindableValue()
{
    Q_D(QmlBindableValue);
    if(d->mePtr)
        *(d->mePtr) = 0;
}

void QmlBindableValue::setTarget(const QmlMetaProperty &prop)
{
    Q_D(QmlBindableValue);
    d->property = prop;

    update();
}

QmlMetaProperty QmlBindableValue::property() const 
{
   Q_D(const QmlBindableValue);
   return d->property; 
}

void QmlBindableValue::init()
{
    Q_D(QmlBindableValue);

    if (d->inited)
        return;
    d->inited = true;
    update();
}

void QmlBindableValue::setExpression(const QString &expr)
{
    QmlExpression::setExpression(expr);
    update();
}

Q_DECLARE_METATYPE(QList<QObject *>);
void QmlBindableValue::update()
{
    Q_D(QmlBindableValue);

#ifdef Q_ENABLE_PERFORMANCE_LOG
    QFxPerfTimer<QFxPerf::BindableValueUpdate> bu;
#endif
    if (!d->inited)
        return;

    if (!d->updating) {
        d->updating = true;

        if (d->property.propertyCategory() == QmlMetaProperty::List) {
            QVariant value = this->value();
            int listType = QmlMetaType::listType(d->property.propertyType());

            if (value.userType() == qMetaTypeId<QList<QObject *> >()) {
                const QList<QObject *> &list =
                    qvariant_cast<QList<QObject *> >(value);
                QVariant listVar = d->property.read();
                QmlMetaType::clear(listVar);
                for (int ii = 0; ii < list.count(); ++ii) {
                    QVariant v = QmlMetaType::fromObject(list.at(ii), listType);
                    QmlMetaType::append(listVar, v);
                }

            } else if (value.type() == uint(listType) ||
                      value.userType() == listType) {
                QVariant listVar = d->property.read();
                QmlMetaType::clear(listVar);
                QmlMetaType::append(listVar, value);
            }
        } else if (d->property.propertyCategory() == QmlMetaProperty::QmlList) {
            // XXX - optimize!
            QVariant value = this->value();
            QVariant list = d->property.read();
            QmlPrivate::ListInterface *li =
                *(QmlPrivate::ListInterface **)list.constData();

            int type = li->type();

            if (QObject *obj = QmlMetaType::toQObject(value)) {
                const QMetaObject *mo =
                    QmlMetaType::rawMetaObjectForType(type);

                const QMetaObject *objMo = obj->metaObject();
                bool found = false;
                while(!found && objMo) {
                    if (objMo == mo)
                        found = true;
                    else
                        objMo = objMo->superClass();
                }

                if (!found) {
                    qWarning() << "Unable to assign object to list";
                    return;
                }

                // NOTE: This assumes a cast to QObject does not alter
                // the object pointer
                void *d = (void *)&obj;
                li->append(d);
            }
        } else if (d->property.propertyCategory() == QmlMetaProperty::Bindable) {

            int idx = d->property.coreIndex();
            Q_ASSERT(idx != -1);

            void *a[1];
            QmlBindableValue *t = this;
            a[0] = (void *)&t;
            QMetaObject::metacall(d->property.object(), 
                                  QMetaObject::WriteProperty,
                                  idx, a);

        } else if (d->property.propertyCategory() == QmlMetaProperty::Object) {

            QVariant value = this->value();

            QObject *obj = QmlMetaType::toQObject(value);

            if (!obj) {
                if (scriptWarnings())
                    qWarning() << "QmlBindableValue: Unable to assign non-object to object property";
                return;
            }

            // XXX This isn't type safe

            // NOTE: We assume that only core properties can have
            // propertyType == Object
            int idx = d->property.coreIndex();
            Q_ASSERT(idx != -1);

            void *a[1];
            a[0] = (void *)&obj;
            d->property.object()->qt_metacall(QMetaObject::WriteProperty,
                                            idx, a);

        } else if (d->property.propertyCategory() == QmlMetaProperty::Normal) {
            QVariant value = this->value();
            if (d->property.propertyType() == QVariant::Url && 
                value.canConvert(QVariant::String) && !value.isNull()) {
                // Must resolve first
                value.setValue(context()->resolvedUrl(value.toString()));
            }
            d->property.write(value);
        }

        d->updating = false;
    } else {
        qmlInfo(d->property.object()) << "Binding loop detected for property" << d->property.name();
    }
}

void QmlBindableValue::valueChanged()
{
    update();
}

QT_END_NAMESPACE
