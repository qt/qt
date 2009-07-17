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

#include "qlistmodelinterface.h"
#include "qfxitem.h"
#include <qmlcontext.h>
#include <qmlexpression.h>
#include "qmlpackage.h"
#include "qhash.h"
#include "qlist.h"
#include "private/qobject_p.h"
#include "private/qmetaobjectbuilder_p.h"
#include "qmlopenmetaobject.h"
#include "qmllistaccessor.h"
#include "qfxvisualitemmodel.h"
#include <QtCore/qdebug.h>

QML_DECLARE_TYPE(QListModelInterface)

QT_BEGIN_NAMESPACE

class QFxVisualItemModelParts;
class QFxVisualItemModelData;
class QFxVisualItemModelPrivate : public QObjectPrivate
{
public:
    QFxVisualItemModelPrivate(QmlContext *);

    QListModelInterface *m_listModelInterface;
    QAbstractItemModel *m_abstractItemModel;
    QFxVisualItemModel *m_visualItemModel;
    QString m_part;

    QmlComponent *m_delegate;
    QmlContext *m_context;
    QList<int> m_roles;
    QHash<int,QString> m_roleNames;

    struct ObjectRef {
        ObjectRef(QObject *object=0) : obj(object), ref(1) {}
        QObject *obj;
        int ref;
    };
    class Cache : public QHash<int, ObjectRef> {
    public:
        QObject *getItem(int index) {
            QObject *item = 0;
            QHash<int,ObjectRef>::iterator it = find(index);
            if (it != end()) {
                (*it).ref++;
                item = (*it).obj;
            }
            return item;
        }
        QObject *item(int index) {
            QObject *item = 0;
            QHash<int, ObjectRef>::const_iterator it = find(index);
            if (it != end())
                item = (*it).obj;
            return item;
        }
        void insertItem(int index, QObject *obj) {
            insert(index, ObjectRef(obj));
        }
        bool releaseItem(QObject *obj) {
            QHash<int, ObjectRef>::iterator it = begin();
            for (; it != end(); ++it) {
                ObjectRef &objRef = *it;
                if (objRef.obj == obj) {
                    if (--objRef.ref == 0) {
                        erase(it);
                        return true;
                    }
                    break;
                }
            }
            return false;
        }
    };

    Cache m_cache;
    QHash<QObject *, QmlPackage*> m_packaged;

    QFxVisualItemModelParts *m_parts;
    friend class QFxVisualItemParts;

    QFxVisualItemModelData *data(QObject *item); 

    QVariant m_modelVariant;
    QmlListAccessor *m_modelList;

    int modelCount() const {
        if (m_visualItemModel)
            return m_visualItemModel->count();
        if (m_listModelInterface)
            return m_listModelInterface->count();
        if (m_abstractItemModel)
            return m_abstractItemModel->rowCount();
        if (m_modelList)
            return m_modelList->count();
        return 0;
    }
};

class QFxVisualItemModelDataMetaObject : public QmlOpenMetaObject
{
public:
    QFxVisualItemModelDataMetaObject(QObject *parent)
    : QmlOpenMetaObject(parent) {}

    virtual QVariant propertyCreated(int, QMetaPropertyBuilder &);
    virtual int createProperty(const char *, const char *);

private:
    friend class QFxVisualItemModelData;
    QList<int> roles;
};

class QFxVisualItemModelData : public QObject
{
Q_OBJECT
public:
    QFxVisualItemModelData(int index, QFxVisualItemModelPrivate *model);

    Q_PROPERTY(int index READ index NOTIFY indexChanged);
    int index() const;
    void setIndex(int index);

    int count() const;
    int role(int) const;
    void setValue(int, const QVariant &);

Q_SIGNALS:
    void indexChanged();

private:
    friend class QFxVisualItemModelDataMetaObject;
    int m_index;
    QFxVisualItemModelPrivate *m_model;
    QFxVisualItemModelDataMetaObject *m_meta;
};

int QFxVisualItemModelData::count() const
{
    return m_meta->count();
}

int QFxVisualItemModelData::role(int id) const
{
    Q_ASSERT(id >= 0 && id < count());
    return m_meta->roles.at(id);
}

void QFxVisualItemModelData::setValue(int id, const QVariant &val)
{
    m_meta->setValue(id, val);
}

int QFxVisualItemModelDataMetaObject::createProperty(const char *name, const char *type)
{
    QFxVisualItemModelData *data = 
        static_cast<QFxVisualItemModelData *>(object());

    if ((!data->m_model->m_listModelInterface || !data->m_model->m_abstractItemModel)
            && data->m_model->m_modelList) {
        if (!qstrcmp(name, "modelData"))
            return QmlOpenMetaObject::createProperty(name, type);
    } else {
        const QLatin1String sname(name);
        for (QHash<int, QString>::ConstIterator iter = data->m_model->m_roleNames.begin();
            iter != data->m_model->m_roleNames.end(); ++iter) {

            if (*iter == sname) 
                return QmlOpenMetaObject::createProperty(name, type);
        }
    }
    return -1;
}

QVariant 
QFxVisualItemModelDataMetaObject::propertyCreated(int, QMetaPropertyBuilder &prop)
{
    prop.setWritable(false);

    QFxVisualItemModelData *data = 
        static_cast<QFxVisualItemModelData *>(object());
    QString name = QLatin1String(prop.name());
    if ((!data->m_model->m_listModelInterface || !data->m_model->m_abstractItemModel)
            && data->m_model->m_modelList) {
        return data->m_model->m_modelList->at(data->m_index);
    } else if (data->m_model->m_listModelInterface) {
        for (QHash<int, QString>::ConstIterator iter = data->m_model->m_roleNames.begin();
            iter != data->m_model->m_roleNames.end(); ++iter) {

            if (*iter == name) {
                roles.append(iter.key());
                QHash<int,QVariant> values = data->m_model->m_listModelInterface->data(data->m_index, QList<int>() << iter.key());
                if (values.isEmpty())
                    return QVariant();
                else
                    return values.value(iter.key());
            } 
        }
    } else if (data->m_model->m_abstractItemModel) {
        for (QHash<int, QString>::ConstIterator iter = data->m_model->m_roleNames.begin();
            iter != data->m_model->m_roleNames.end(); ++iter) {

            if (*iter == name) {
                roles.append(iter.key());
                QModelIndex index = data->m_model->m_abstractItemModel->index(data->m_index, 0);
                return data->m_model->m_abstractItemModel->data(index, iter.key());
            } 
        }
    }
    Q_ASSERT(!"Can never be reached");
    return QVariant();
}

QFxVisualItemModelData::QFxVisualItemModelData(int index,  
                                               QFxVisualItemModelPrivate *model)
: m_index(index), m_model(model), 
  m_meta(new QFxVisualItemModelDataMetaObject(this))
{
}

int QFxVisualItemModelData::index() const
{
    return m_index;
}

// This is internal only - it should not be set from qml
void QFxVisualItemModelData::setIndex(int index)
{
    m_index = index;
    emit indexChanged();
}

class QFxVisualItemModelPartsMetaObject : public QmlOpenMetaObject
{
public:
    QFxVisualItemModelPartsMetaObject(QObject *parent)
    : QmlOpenMetaObject(parent) {}

    virtual QVariant propertyCreated(int, QMetaPropertyBuilder &);
};

class QFxVisualItemModelParts : public QObject
{
Q_OBJECT
public:
    QFxVisualItemModelParts(QFxVisualItemModel *parent);

private:
    friend class QFxVisualItemModelPartsMetaObject;
    QFxVisualItemModel *model;
};

QVariant 
QFxVisualItemModelPartsMetaObject::propertyCreated(int, QMetaPropertyBuilder &prop)
{
    prop.setWritable(false);

    QFxVisualItemModel *m = new QFxVisualItemModel;
    m->setParent(object());
    m->setPart(QLatin1String(prop.name()));
    m->setModel(QVariant::fromValue(static_cast<QFxVisualItemModelParts *>(object())->model));

    QVariant var = QVariant::fromValue((QObject *)m);
    return var;
}

QFxVisualItemModelParts::QFxVisualItemModelParts(QFxVisualItemModel *parent)
: QObject(parent), model(parent) 
{
    new QFxVisualItemModelPartsMetaObject(this);
}

QFxVisualItemModelPrivate::QFxVisualItemModelPrivate(QmlContext *ctxt)
: m_listModelInterface(0), m_abstractItemModel(0), m_visualItemModel(0), m_delegate(0)
, m_context(ctxt), m_parts(0), m_modelList(0)
{
}

QFxVisualItemModelData *QFxVisualItemModelPrivate::data(QObject *item)
{
    QList<QFxVisualItemModelData *> dataList = 
        item->findChildren<QFxVisualItemModelData *>();
    Q_ASSERT(dataList.count() == 1);
    return dataList.first();
}

QFxVisualItemModel::QFxVisualItemModel()
: QObject(*(new QFxVisualItemModelPrivate(0)))
{
}

QFxVisualItemModel::QFxVisualItemModel(QmlContext *ctxt)
: QObject(*(new QFxVisualItemModelPrivate(ctxt)))
{
}

QFxVisualItemModel::~QFxVisualItemModel()
{
    Q_D(QFxVisualItemModel);
    if (d->m_modelList)
        delete d->m_modelList;
}

QVariant QFxVisualItemModel::model() const
{
    Q_D(const QFxVisualItemModel);
    return d->m_modelVariant;
}

void QFxVisualItemModel::setModel(const QVariant &model)
{
    Q_D(QFxVisualItemModel);
    d->m_modelVariant = model;
    if (d->m_listModelInterface) {
        // Assume caller has released all items.
        QObject::disconnect(d->m_listModelInterface, SIGNAL(itemsChanged(int,int,QList<int>)),
                this, SLOT(_q_itemsChanged(int,int,QList<int>)));
        QObject::disconnect(d->m_listModelInterface, SIGNAL(itemsInserted(int,int)),
                this, SLOT(_q_itemsInserted(int,int)));
        QObject::disconnect(d->m_listModelInterface, SIGNAL(itemsRemoved(int,int)),
                this, SLOT(_q_itemsRemoved(int,int)));
        QObject::disconnect(d->m_listModelInterface, SIGNAL(itemsMoved(int,int,int)),
                this, SLOT(_q_itemsMoved(int,int,int)));
        d->m_listModelInterface = 0;
    } else if (d->m_abstractItemModel) {
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsInserted(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsInserted(const QModelIndex &,int,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsRemoved(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsRemoved(const QModelIndex &,int,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                            this, SLOT(_q_dataChanged(const QModelIndex&,const QModelIndex&)));
    } else if (d->m_visualItemModel) {
        QObject::disconnect(d->m_visualItemModel, SIGNAL(itemsInserted(int,int)),
                         this, SIGNAL(itemsInserted(int,int)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(itemsRemoved(int,int)),
                         this, SIGNAL(itemsRemoved(int,int)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(itemsMoved(int,int,int)),
                         this, SIGNAL(itemsMoved(int,int,int)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(createdPackage(int,QmlPackage*)),
                         this, SLOT(_q_createdPackage(int,QmlPackage*)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(destroyingPackage(QmlPackage*)),
                         this, SLOT(_q_destroyingPackage(QmlPackage*)));
        d->m_visualItemModel = 0;
    }

    QObject *object = qvariant_cast<QObject *>(model);
    if (object && (d->m_listModelInterface = qobject_cast<QListModelInterface *>(object))) {
        d->m_roles.clear();
        d->m_roleNames.clear();
        if (d->m_listModelInterface) {
            d->m_roles = d->m_listModelInterface->roles();
            for (int ii = 0; ii < d->m_roles.count(); ++ii) 
                d->m_roleNames.insert(d->m_roles.at(ii), 
                                      d->m_listModelInterface->toString(d->m_roles.at(ii)));
        }

        QObject::connect(d->m_listModelInterface, SIGNAL(itemsChanged(int,int,QList<int>)),
                         this, SLOT(_q_itemsChanged(int,int,QList<int>)));
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsInserted(int,int)),
                         this, SLOT(_q_itemsInserted(int,int)));
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsRemoved(int,int)),
                         this, SLOT(_q_itemsRemoved(int,int)));
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsMoved(int,int,int)),
                         this, SLOT(_q_itemsMoved(int,int,int)));

        if (d->m_delegate && d->m_listModelInterface->count())
            emit itemsInserted(0, d->m_listModelInterface->count());
        return;
    } else if (object && (d->m_abstractItemModel = qobject_cast<QAbstractItemModel *>(object))) {
        d->m_roles.clear();
        d->m_roleNames.clear();
        for (QHash<int,QByteArray>::const_iterator it = d->m_abstractItemModel->roleNames().begin();
                it != d->m_abstractItemModel->roleNames().end(); ++it) {
            d->m_roles.append(it.key());
            d->m_roleNames.insert(it.key(), QLatin1String(*it));
        }
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsInserted(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsInserted(const QModelIndex &,int,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsRemoved(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsRemoved(const QModelIndex &,int,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                            this, SLOT(_q_dataChanged(const QModelIndex&,const QModelIndex&)));
        return;
    }
    if ((d->m_visualItemModel = qvariant_cast<QFxVisualItemModel *>(model))) {
        QObject::connect(d->m_visualItemModel, SIGNAL(itemsInserted(int,int)),
                         this, SIGNAL(itemsInserted(int,int)));
        QObject::connect(d->m_visualItemModel, SIGNAL(itemsRemoved(int,int)),
                         this, SIGNAL(itemsRemoved(int,int)));
        QObject::connect(d->m_visualItemModel, SIGNAL(itemsMoved(int,int,int)),
                         this, SIGNAL(itemsMoved(int,int,int)));
        QObject::connect(d->m_visualItemModel, SIGNAL(createdPackage(int,QmlPackage*)),
                         this, SLOT(_q_createdPackage(int,QmlPackage*)));
        QObject::connect(d->m_visualItemModel, SIGNAL(destroyingPackage(QmlPackage*)),
                         this, SLOT(_q_destroyingPackage(QmlPackage*)));
        return;
    }
    if (!d->m_modelList)
        d->m_modelList = new QmlListAccessor;
    d->m_modelList->setList(model);
    if (d->m_delegate && d->modelCount())
        emit itemsInserted(0, d->modelCount());
}

QmlComponent *QFxVisualItemModel::delegate() const
{
    Q_D(const QFxVisualItemModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->delegate();
    return d->m_delegate;
}

void QFxVisualItemModel::setDelegate(QmlComponent *delegate)
{
    Q_D(QFxVisualItemModel);
    d->m_delegate = delegate;
    if (d->modelCount())
        emit itemsInserted(0, d->modelCount());
}

QString QFxVisualItemModel::part() const
{
    Q_D(const QFxVisualItemModel);
    return d->m_part;
}

void QFxVisualItemModel::setPart(const QString &part)
{
    Q_D(QFxVisualItemModel);
    d->m_part = part;
}

int QFxVisualItemModel::count() const
{
    Q_D(const QFxVisualItemModel);
    return d->modelCount();
}

QFxItem *QFxVisualItemModel::item(int index, bool complete)
{
    Q_D(QFxVisualItemModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, d->m_part.toLatin1(), complete);
    return item(index, QByteArray(), complete);
}

/*
  Returns ReleaseStatus flags.
*/
QFxVisualItemModel::ReleaseFlags QFxVisualItemModel::release(QFxItem *item)
{
    Q_D(QFxVisualItemModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->release(item);

    ReleaseFlags stat = 0;
    QObject *obj = item;
    bool inPackage = false;

    QHash<QObject*,QmlPackage*>::iterator it = d->m_packaged.find(item);
    if (it != d->m_packaged.end()) {
        QmlPackage *package = *it;
        d->m_packaged.erase(it);
        if (d->m_packaged.contains(item))
            stat |= Referenced;
        inPackage = true;
        obj = package; // fall through and delete
    }

    if (d->m_cache.releaseItem(obj)) {
        if (inPackage)
            emit destroyingPackage(qobject_cast<QmlPackage*>(obj));
        stat |= Destroyed;
        delete obj;
    } else if (!inPackage) {
        stat |= Referenced;
    }

    return stat;
}

QObject *QFxVisualItemModel::parts() 
{
    Q_D(QFxVisualItemModel);
    if (!d->m_parts) 
        d->m_parts = new QFxVisualItemModelParts(this);
    return d->m_parts;
}

QFxItem *QFxVisualItemModel::item(int index, const QByteArray &viewId, bool complete)
{
    Q_D(QFxVisualItemModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, viewId, complete);

    if (d->modelCount() <= 0 || !d->m_delegate)
        return 0;

    QObject *nobj = d->m_cache.getItem(index);
    if (!nobj) {
        QmlContext *ccontext = d->m_context;
        if (!ccontext) ccontext = qmlContext(this);
        QmlContext *ctxt = new QmlContext(ccontext);
        QFxVisualItemModelData *data = new QFxVisualItemModelData(index, d);
        ctxt->setContextProperty(QLatin1String("model"), data);
        ctxt->addDefaultObject(data);
        nobj = d->m_delegate->beginCreate(ctxt);
        if (complete)
            d->m_delegate->completeCreate();
        if (nobj) {
            ctxt->setParent(nobj);
            data->setParent(nobj);
            d->m_cache.insertItem(index, nobj);
            if (QmlPackage *package = qobject_cast<QmlPackage *>(nobj))
                emit createdPackage(index, package);
        } else {
            delete data;
            delete ctxt;
            qWarning() << d->m_delegate->errors();
        }
    }
    QFxItem *item = qobject_cast<QFxItem *>(nobj);
    if (!item) {
        QmlPackage *package = qobject_cast<QmlPackage *>(nobj);
        if (package) {
            QObject *o = package->part(QLatin1String(viewId));
            item = qobject_cast<QFxItem *>(o);
            d->m_packaged.insertMulti(item, package);
        }
    }

    return item;
}

void QFxVisualItemModel::completeItem()
{
    Q_D(QFxVisualItemModel);
    if (d->m_visualItemModel) {
        d->m_visualItemModel->completeItem();
        return;
    }

    d->m_delegate->completeCreate();
}

QVariant QFxVisualItemModel::evaluate(int index, const QString &expression, QObject *objectContext)
{
    Q_D(QFxVisualItemModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->evaluate(index, expression, objectContext);

    if ((!d->m_listModelInterface && !d->m_abstractItemModel) || !d->m_delegate)
        return QVariant();

    QVariant value;
    QObject *nobj = d->m_cache.item(index);
    if (nobj) {
        QFxItem *item = qobject_cast<QFxItem *>(nobj);
        if (item) {
            QmlExpression e(qmlContext(item), expression, objectContext);
            e.setTrackChange(false);
            value = e.value();
        }
    } else {
        QmlContext *ccontext = d->m_context;
        if (!ccontext) ccontext = qmlContext(this);
        QmlContext *ctxt = new QmlContext(ccontext);
        QFxVisualItemModelData *data = new QFxVisualItemModelData(index, d);
        ctxt->addDefaultObject(data);
        QmlExpression e(ctxt, expression, objectContext);
        e.setTrackChange(false);
        value = e.value();
        delete data;
        delete ctxt;
    }

    return value;
}

int QFxVisualItemModel::indexOf(QFxItem *item, QObject *objectContext) const
{
    QmlExpression e(qmlContext(item), QLatin1String("index"), objectContext);
    e.setTrackChange(false);
    QVariant value = e.value();
    if (value.isValid())
        return value.toInt();
    return -1;
}

void QFxVisualItemModel::_q_itemsChanged(int index, int count, 
                                         const QList<int> &roles)
{
    Q_D(QFxVisualItemModel);
    // XXX - highly inefficient
    for (int ii = index; ii < index + count; ++ii) {

        if (QObject *item = d->m_cache.item(ii)) {
            QFxVisualItemModelData *data = d->data(item);

            for (int prop = 0; prop < data->count(); ++prop) {

                int role = data->role(prop);
                if (roles.contains(role)) {
                    if (d->m_listModelInterface) {
                        data->setValue(prop, d->m_listModelInterface->data(ii, QList<int>() << role).value(role));
                    } else if (d->m_abstractItemModel) {
                        QModelIndex index = d->m_abstractItemModel->index(ii, 0);
                        data->setValue(prop, d->m_abstractItemModel->data(index, role));
                    }
                }
            }
        }

    }
}

void QFxVisualItemModel::_q_itemsInserted(int index, int count)
{
    Q_D(QFxVisualItemModel);
    // XXX - highly inefficient
    QHash<int,QFxVisualItemModelPrivate::ObjectRef> items;
    for (QHash<int,QFxVisualItemModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= index) {
            QFxVisualItemModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() + count;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QFxVisualItemModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsInserted(index, count);
}

void QFxVisualItemModel::_q_itemsRemoved(int index, int count)
{
    Q_D(QFxVisualItemModel);
    // XXX - highly inefficient
    QHash<int, QFxVisualItemModelPrivate::ObjectRef> items;
    for (QHash<int, QFxVisualItemModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {
        if (iter.key() >= index && iter.key() < index + count) {
            QFxVisualItemModelPrivate::ObjectRef objRef = *iter;
            iter = d->m_cache.erase(iter);
            items.insertMulti(-1, objRef); //XXX perhaps better to maintain separately
            QFxVisualItemModelData *data = d->data(objRef.obj);
            data->setIndex(-1);
        } else if (iter.key() >= index + count) {
            QFxVisualItemModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - count;
            iter = d->m_cache.erase(iter);
            items.insert(index, objRef);
            QFxVisualItemModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }

    d->m_cache.unite(items);
    emit itemsRemoved(index, count);
}

void QFxVisualItemModel::_q_itemsMoved(int from, int to, int count)
{
    Q_D(QFxVisualItemModel);
    // XXX - highly inefficient
    QHash<int,QFxVisualItemModelPrivate::ObjectRef> items;
    for (QHash<int,QFxVisualItemModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= from && iter.key() < from + count) {
            QFxVisualItemModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - from + to;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QFxVisualItemModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsMoved(from, to, count);
}

void QFxVisualItemModel::_q_rowsInserted(const QModelIndex &, int begin, int end)
{
    _q_itemsInserted(begin, end - begin + 1);
}

void QFxVisualItemModel::_q_rowsRemoved(const QModelIndex &, int begin, int end)
{
    _q_itemsRemoved(begin, end - begin + 1);
}

void QFxVisualItemModel::_q_dataChanged(const QModelIndex &begin, const QModelIndex &end)
{
    Q_D(QFxVisualItemModel);
    _q_itemsChanged(begin.row(), end.row() - begin.row() + 1, d->m_roles);
}

void QFxVisualItemModel::_q_createdPackage(int index, QmlPackage *package)
{
    Q_D(QFxVisualItemModel);
    emit createdItem(index, qobject_cast<QFxItem*>(package->part(d->m_part)));
}

void QFxVisualItemModel::_q_destroyingPackage(QmlPackage *package)
{
    Q_D(QFxVisualItemModel);
    emit destroyingItem(qobject_cast<QFxItem*>(package->part(d->m_part)));
}

QML_DEFINE_TYPE(QFxVisualItemModel,VisualModel)

QT_END_NAMESPACE
#include "qfxvisualitemmodel.moc"
