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

#include "qfxrepeater.h"
#include "qfxrepeater_p.h"
#include "qmllistaccessor.h"
#include <qlistmodelinterface.h>


QT_BEGIN_NAMESPACE
QFxRepeaterPrivate::QFxRepeaterPrivate()
: component(0)
{
}

QFxRepeaterPrivate::~QFxRepeaterPrivate()
{
}

QFxItem *QFxRepeaterPrivate::addItem(QmlContext *ctxt, QFxItem *lastItem)
{
    Q_Q(QFxRepeater);
    QObject *nobj = component->create(ctxt);
    QFxItem *item = qobject_cast<QFxItem *>(nobj);
    if (item) {
        item->setParent(q->itemParent());
//        item->stackUnder(lastItem);
        deletables << nobj;
    } else {
        delete nobj;
    }

    return item;
}

QML_DEFINE_TYPE(QFxRepeater,Repeater)

/*!
    \qmlclass Repeater
    \inherits Item

    \brief The Repeater item allows you to repeat a component based on a data source.

    The Repeater item is used when you want to create a large number of 
    similar items.  For each entry in the data source, an item is instantiated
    in a context seeded with data from the data source.  If the repeater will
    be instantiating a large number of instances, it may be more efficient to
    use one of Qt Declarative's \l {xmlViews}{view items}.

    The data source may be either an object list, a string list or a Qt model.
    In each case, the data element and the index is exposed to each instantiated
    component.  The index is always exposed as an accessible \c index property.
    In the case of an object or string list, the data element (of type string
    or object) is available as the \c modelData property.  In the case of a Qt model,
    all roles are available as named properties just like in the view classes.

    Items instantiated by the Repeater are inserted, in order, as 
    children of the Repeater's parent.  The insertion starts immediately after
    the repeater's position in its parent stacking list.  This is to allow
    you to use a Repeater inside a layout.  The following QML example shows how
    the instantiated items would visually appear stacked between the red and 
    blue rectangles.

    \snippet doc/src/snippets/declarative/repeater.qml 0

    \image repeater.png

    The repeater instance continues to own all items it instantiates, even
    if they are otherwise manipulated.  It is illegal to manually remove an item
    created by the Repeater.
 */

/*!
    \internal
    \class QFxRepeater
    \ingroup group_utility
    \qmlclass Repeater

    \brief The QFxRepeater class allows you to repeat a component based on a 
           data source.

    The QFxRepeater class is used when you want to create a large number of 
    similar items.  For each entry in the data source, an item is instantiated
    in a context seeded with data from the data source.

    The data source may be either an object list, a string list or a Qt model.
    In each case, the data element and the index is exposed to each instantiated
    component.  The index is always exposed as an accessible \c index property.
    In the case of an object or string list, the data element (of type string
    or object) is available as the \c modelData property.  In the case of a Qt model,
    all roles are available as named properties just like in the view classes.

    As a special case the data source can also be merely a number. In this case it will
    create that many instances of the component. They will also be assigned an index
    based on the order they are created.

    Items instantiated by the QFxRepeater class are inserted, in order, as 
    children of the repeater's parent.  The insertion starts immediately after
    the repeater's position in its parent stacking list.  This is to allow
    you to use a repeater inside a layout.  The following QML example shows how
    the instantiated items would visually appear stacked between the red and 
    blue rectangles.

    \snippet doc/src/snippets/declarative/repeater.qml 0

    The QFxRepeater instance continues to own all items it instantiates, even
    if they are otherwise manipulated.  It is illegal to manually delete an item
    created by the repeater.  On destruction, the repeater will clean up any
    items it has instantiated.


    XXX Repeater is very conservative in how it instatiates/deletes items.  Also
    new model entries will not be created and old ones will not be removed.
 */

/*!
    Create a new QFxRepeater instance.
 */
QFxRepeater::QFxRepeater(QFxItem *parent)
  : QFxItem(*(new QFxRepeaterPrivate), parent)
{
}

/*!
    \internal 
 */
QFxRepeater::QFxRepeater(QFxRepeaterPrivate &dd, QFxItem *parent)
  : QFxItem(dd, parent)
{
}

/*!
    Destroy the repeater instance.  All items it instantiated are also 
    destroyed.
 */
QFxRepeater::~QFxRepeater()
{
}

/*!
    \qmlproperty any Repeater::dataSource

    The Repeater's data source.

    The data source may be either an object list, a string list or a Qt model.
    In each case, the data element and the index is exposed to each instantiated
    component.  The index is always exposed as an accessible \c index property.
    In the case of an object or string list, the data element (of type string
    or object) is available as the \c modelData property.  In the case of a Qt model,
    all roles are available as named properties just like in the view classes.

    As a special case the data source can also be merely a number. In this case it will
    create that many instances of the component. They will also be assigned an index
    based on the order they are created.
*/

/*!
    \property QFxRepeater::dataSource
    \brief The source of data for the repeater.
 */
QVariant QFxRepeater::dataSource() const
{
    return QVariant();
}

void QFxRepeater::setDataSource(const QVariant &v)
{
    Q_D(QFxRepeater);
    d->dataSource = v;
    regenerate();
}

/*!
    \qmlproperty Component Repeater::component
    \default

    The component to repeat.
 */
/*!
    \property QFxRepeater::component
    \brief The component to repeat.
 */
QmlComponent *QFxRepeater::component() const
{
    Q_D(const QFxRepeater);
    return d->component;
}

void QFxRepeater::setComponent(QmlComponent *_c)
{
    Q_D(QFxRepeater);
    d->component = _c;
    regenerate();
}

/*!
    \internal
 */
void QFxRepeater::componentComplete()
{
    QFxItem::componentComplete();
    regenerate();
}

/*!
    \internal
 */
void QFxRepeater::parentChanged(QFxItem *o, QFxItem *n)
{
    QFxItem::parentChanged(o, n);
    regenerate();
}

/*!
    \internal
 */
void QFxRepeater::regenerate()
{
    Q_D(QFxRepeater);
    
    qDeleteAll(d->deletables); 
    d->deletables.clear();
    if (!d->component || !itemParent() || !isComponentComplete())
        return;

    QFxItem *lastItem = this;

    if (d->dataSource.type() == QVariant::StringList) {
        QStringList sl = qvariant_cast<QStringList>(d->dataSource);

        for (int ii = 0; ii < sl.size(); ++ii) {
            QmlContext *ctxt = new QmlContext(qmlContext(this), this);
            d->deletables << ctxt;

            ctxt->setContextProperty(QLatin1String("index"), ii);
            ctxt->setContextProperty(QLatin1String("modelData"), sl.at(ii));

            if (QFxItem *item = d->addItem(ctxt, lastItem))
                lastItem = item;
        }
    } else if (QmlMetaType::isList(d->dataSource)) {
        int cnt = QmlMetaType::listCount(d->dataSource);
        if (cnt <= 0)
            return;

        for (int ii = 0; ii < cnt; ++ii) {
            QVariant v = QmlMetaType::listAt(d->dataSource, ii);
            QObject *o = QmlMetaType::toQObject(v);

            QmlContext *ctxt = new QmlContext(qmlContext(this), this);
            d->deletables << ctxt;

            ctxt->setContextProperty(QLatin1String("index"), ii);
            ctxt->setContextProperty(QLatin1String("modelData"), o);

            if (QFxItem *item = d->addItem(ctxt, lastItem))
                lastItem = item;
        }
    } else if (QListModelInterface *model = qobject_cast<QListModelInterface*>(d->dataSource.value<QObject*>())) {
        int cnt = model->count();
        if (cnt <= 0)
            return;

        for (int ii = 0; ii < cnt; ++ii) {
            QmlContext *ctxt = new QmlContext(qmlContext(this), this);
            d->deletables << ctxt;

            ctxt->setContextProperty(QLatin1String("index"), ii);

            QList<int> roles = model->roles();
            QHash<int,QVariant> data = model->data(ii,roles);
            for (int j = 0; j < roles.size(); ++j) {
                ctxt->setContextProperty(model->toString(roles.at(j)), data.value(roles.at(j)));
            }

            //for compatability with other lists, assign data if there is only a single role
            if (roles.size() == 1)
                ctxt->setContextProperty(QLatin1String("modelData"), data.value(roles.at(0)));

            if (QFxItem *item = d->addItem(ctxt, lastItem))
                lastItem = item;
        }
    } else if (QObject *object = d->dataSource.value<QObject*>()) {
        // A single object (i.e. list of size 1).
        // Properties are the roles (excluding objectName).
        QmlContext *ctxt = new QmlContext(qmlContext(this), this);
        d->deletables << ctxt;

        ctxt->setContextProperty(QLatin1String("index"), QVariant(0));
        for (int ii = 1; ii < object->metaObject()->propertyCount(); ++ii) {
            const QMetaProperty &prop = object->metaObject()->property(ii);
            ctxt->setContextProperty(QLatin1String(prop.name()), prop.read(object));
        }

        //for compatability with other lists, assign data if there is only a single role (excluding objectName)
        if (object->metaObject()->propertyCount() == 2) {
            const QMetaProperty &prop = object->metaObject()->property(1);
            ctxt->setContextProperty(QLatin1String("modelData"), prop.read(object));
        }

        d->addItem(ctxt, lastItem);

    } else if (d->dataSource.canConvert(QVariant::Int)){

        int count = qvariant_cast<int>(d->dataSource);

        for (int ii = 0; ii < count; ++ii) {
            QmlContext *ctxt = new QmlContext(qmlContext(this), this);
            d->deletables << ctxt;

            ctxt->setContextProperty(QLatin1String("index"), ii);

           if (QFxItem *item = d->addItem(ctxt, lastItem))
                lastItem = item;
        }
    }
}
QT_END_NAMESPACE
