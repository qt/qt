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

#include "qfxcomponentinstance.h"
#include "qfxcomponentinstance_p.h"
#include <private/qfxperf_p.h>
#include <QtDeclarative/qmlinfo.h>


QT_BEGIN_NAMESPACE
QML_DEFINE_TYPE(QFxComponentInstance,ComponentInstance)

/*!
    \internal
    \class QFxComponentInstance ComponentInstance

    \brief The QFxComponentInstance class provides a way to instantiate an item from a component.
 */

/*!
    \qmlclass ComponentInstance QFxComponentInstance
    \brief The ComponentInstance item allows you to instantiate a \l{Component}.

    \qml
    Item {
        Component {
            id: RedSquare
            Rect { color: "red"; width: 10; height: 10 }
        }

        ComponentInstance { component: RedSquare }
    }
    \endqml
*/
QFxComponentInstance::QFxComponentInstance(QFxItem *parent)
  : QFxItem(*(new QFxComponentInstancePrivate), parent)
{
    setOptions(IsFocusRealm);
}

QFxComponentInstance::QFxComponentInstance(QFxComponentInstancePrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
    setOptions(IsFocusRealm);
}

/*!
    \qmlproperty Component QFxComponentInstance::component

    This property holds the component to instantiate.
*/
QmlComponent *QFxComponentInstance::component() const
{
    Q_D(const QFxComponentInstance);
    return d->component;
}

void QFxComponentInstance::setComponent(QmlComponent *c)
{
    Q_D(QFxComponentInstance);
    if (d->component) {
        qmlInfo(this) << "component is a write-once property.";
        return;
    }
    d->component = c;
    create();
}

void QFxComponentInstance::create()
{
    Q_D(QFxComponentInstance);
    if (d->component) {
        QObject *obj= d->component->create(qmlContext(this));
        if (obj) {
            QFxItem *objitem = qobject_cast<QFxItem *>(obj);
            if (objitem) {
                d->instance = objitem;
                objitem->setItemParent(this);
                objitem->setFocus(true);
                connect(objitem, SIGNAL(widthChanged()), this, SLOT(updateSize()));
                connect(objitem, SIGNAL(heightChanged()), this, SLOT(updateSize()));
                updateSize();
                emit instanceChanged();
            } else {
                delete obj;
            }
        }   
    }
}

void QFxComponentInstance::updateSize()
{
    QFxItem *i = instance();
    if (i) {
        if (!widthValid())
            setImplicitWidth(i->width());
        if (!heightValid())
            setImplicitHeight(i->height());
    }
}

/*!
    \qmlproperty Item QFxComponentInstance::instance

    This property holds the instantiated component.
*/
QFxItem *QFxComponentInstance::instance() const
{
    Q_D(const QFxComponentInstance);
    return d->instance;
}

QT_END_NAMESPACE
