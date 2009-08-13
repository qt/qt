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
#include "private/qguard_p.h"
#include <QtCore/qdebug.h>

QML_DECLARE_TYPE(QListModelInterface)

QT_BEGIN_NAMESPACE

class QFxVisualItemModelAttached : public QObject
{
    Q_OBJECT

public:
    QFxVisualItemModelAttached(QObject *parent)
        : QObject(parent), m_index(0) {}
    ~QFxVisualItemModelAttached() {
        attachedProperties.remove(parent());
    }

    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    int index() const { return m_index; }
    void setIndex(int idx) {
        if (m_index != idx) {
            m_index = idx;
            emit indexChanged();
        }
    }

    static QFxVisualItemModelAttached *properties(QObject *obj) {
        QFxVisualItemModelAttached *rv = attachedProperties.value(obj);
        if (!rv) {
            rv = new QFxVisualItemModelAttached(obj);
            attachedProperties.insert(obj, rv);
        }
        return rv;
    }

signals:
    void indexChanged();

public:
    int m_index;

    static QHash<QObject*, QFxVisualItemModelAttached*> attachedProperties;
};

QHash<QObject*, QFxVisualItemModelAttached*> QFxVisualItemModelAttached::attachedProperties;


class QFxVisualItemModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QFxVisualItemModel);
public:
    QFxVisualItemModelPrivate() : QObjectPrivate(), children(this) {}

    struct ItemList : public QmlConcreteList<QFxItem *>
    {
        ItemList(QFxVisualItemModelPrivate *m) : QmlConcreteList<QFxItem *>(), model(m) {}

        void append(QFxItem *item);

        QFxVisualItemModelPrivate *model;
    };

    void itemAppended() {
        Q_Q(QFxVisualItemModel);
        QFxVisualItemModelAttached *attached = QFxVisualItemModelAttached::properties(children.last());
        attached->setIndex(children.count()-1);
        emit q->itemsInserted(children.count()-1, 1);
        emit q->countChanged();
    }

    ItemList children;
};


/*!
    \qmlclass VisualItemModel QFxVisualItemModel
    \brief The VisualItemModel allows items to be provided to a view.

    The children of the VisualItemModel are provided in a model which
    can be used in a view.  An item can determine its index within the
    model via the VisualItemModel.index attached property.

    The example below places three colored rectangles in a ListView.
    \code
    Item {
        VisualItemModel {
            id: ItemModel
            Rect { height: 30; width: 80; color: "red" }
            Rect { height: 30; width: 80; color: "green" }
            Rect { height: 30; width: 80; color: "blue" }
        }

        ListView {
            anchors.fill: parent
            model: ItemModel
        }
    }
    \endcode
*/
QFxVisualItemModel::QFxVisualItemModel()
    : QFxVisualModel(*(new QFxVisualItemModelPrivate))
{
}

QmlList<QFxItem *> *QFxVisualItemModel::children()
{
    Q_D(QFxVisualItemModel);
    return &(d->children);
}

/*!
    \qmlproperty int VisualItemModel::count

    The number of items in the model.  This property is readonly.
*/
int QFxVisualItemModel::count() const
{
    Q_D(const QFxVisualItemModel);
    return d->children.count();
}

bool QFxVisualItemModel::isValid() const
{
    return true;
}

QFxItem *QFxVisualItemModel::item(int index, bool)
{
    Q_D(QFxVisualItemModel);
    return d->children.at(index);
}

QFxVisualModel::ReleaseFlags QFxVisualItemModel::release(QFxItem *)
{
    // Nothing to do
    return 0;
}

void QFxVisualItemModel::completeItem()
{
    // Nothing to do
}

QVariant QFxVisualItemModel::evaluate(int index, const QString &expression, QObject *objectContext)
{
    Q_D(QFxVisualItemModel);
    QmlContext *ccontext = qmlContext(this);
    QmlContext *ctxt = new QmlContext(ccontext);
    ctxt->addDefaultObject(d->children.at(index));
    QmlExpression e(ctxt, expression, objectContext);
    e.setTrackChange(false);
    QVariant value = e.value();
    delete ctxt;
    return value;
}

int QFxVisualItemModel::indexOf(QFxItem *item, QObject *) const
{
    Q_D(const QFxVisualItemModel);
    return d->children.indexOf(item);
}

void QFxVisualItemModelPrivate::ItemList::append(QFxItem *item)
{
    QmlConcreteList<QFxItem*>::append(item);
    item->QObject::setParent(model->q_ptr);
    model->itemAppended();
}

QFxVisualItemModelAttached *QFxVisualItemModel::qmlAttachedProperties(QObject *obj)
{
    return QFxVisualItemModelAttached::properties(obj);
}


class QFxVisualDataModelParts;
class QFxVisualDataModelData;
class QFxVisualDataModelPrivate : public QObjectPrivate
{
public:
    QFxVisualDataModelPrivate(QmlContext *);

    QGuard<QListModelInterface> m_listModelInterface;
    QGuard<QAbstractItemModel> m_abstractItemModel;
    QGuard<QFxVisualDataModel> m_visualItemModel;
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

    QFxVisualDataModelParts *m_parts;
    friend class QFxVisualItemParts;

    QFxVisualDataModelData *data(QObject *item);

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

class QFxVisualDataModelDataMetaObject : public QmlOpenMetaObject
{
public:
    QFxVisualDataModelDataMetaObject(QObject *parent)
    : QmlOpenMetaObject(parent) {}

    virtual QVariant propertyCreated(int, QMetaPropertyBuilder &);
    virtual int createProperty(const char *, const char *);

private:
    friend class QFxVisualDataModelData;
    QList<int> roles;
};

class QFxVisualDataModelData : public QObject
{
Q_OBJECT
public:
    QFxVisualDataModelData(int index, QFxVisualDataModelPrivate *model);

    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    int index() const;
    void setIndex(int index);

    int count() const;
    int role(int) const;
    void setValue(int, const QVariant &);

Q_SIGNALS:
    void indexChanged();

private:
    friend class QFxVisualDataModelDataMetaObject;
    int m_index;
    QFxVisualDataModelPrivate *m_model;
    QFxVisualDataModelDataMetaObject *m_meta;
};

int QFxVisualDataModelData::count() const
{
    return m_meta->count();
}

int QFxVisualDataModelData::role(int id) const
{
    Q_ASSERT(id >= 0 && id < count());
    return m_meta->roles.at(id);
}

void QFxVisualDataModelData::setValue(int id, const QVariant &val)
{
    m_meta->setValue(id, val);
}

int QFxVisualDataModelDataMetaObject::createProperty(const char *name, const char *type)
{
    QFxVisualDataModelData *data =
        static_cast<QFxVisualDataModelData *>(object());

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
QFxVisualDataModelDataMetaObject::propertyCreated(int, QMetaPropertyBuilder &prop)
{
    prop.setWritable(false);

    QFxVisualDataModelData *data =
        static_cast<QFxVisualDataModelData *>(object());
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

QFxVisualDataModelData::QFxVisualDataModelData(int index,
                                               QFxVisualDataModelPrivate *model)
: m_index(index), m_model(model), 
  m_meta(new QFxVisualDataModelDataMetaObject(this))
{
}

int QFxVisualDataModelData::index() const
{
    return m_index;
}

// This is internal only - it should not be set from qml
void QFxVisualDataModelData::setIndex(int index)
{
    m_index = index;
    emit indexChanged();
}

class QFxVisualDataModelPartsMetaObject : public QmlOpenMetaObject
{
public:
    QFxVisualDataModelPartsMetaObject(QObject *parent)
    : QmlOpenMetaObject(parent) {}

    virtual QVariant propertyCreated(int, QMetaPropertyBuilder &);
};

class QFxVisualDataModelParts : public QObject
{
Q_OBJECT
public:
    QFxVisualDataModelParts(QFxVisualDataModel *parent);

private:
    friend class QFxVisualDataModelPartsMetaObject;
    QFxVisualDataModel *model;
};

QVariant 
QFxVisualDataModelPartsMetaObject::propertyCreated(int, QMetaPropertyBuilder &prop)
{
    prop.setWritable(false);

    QFxVisualDataModel *m = new QFxVisualDataModel;
    m->setParent(object());
    m->setPart(QLatin1String(prop.name()));
    m->setModel(QVariant::fromValue(static_cast<QFxVisualDataModelParts *>(object())->model));

    QVariant var = QVariant::fromValue((QObject *)m);
    return var;
}

QFxVisualDataModelParts::QFxVisualDataModelParts(QFxVisualDataModel *parent)
: QObject(parent), model(parent) 
{
    new QFxVisualDataModelPartsMetaObject(this);
}

QFxVisualDataModelPrivate::QFxVisualDataModelPrivate(QmlContext *ctxt)
: m_listModelInterface(0), m_abstractItemModel(0), m_visualItemModel(0), m_delegate(0)
, m_context(ctxt), m_parts(0), m_modelList(0)
{
}

QFxVisualDataModelData *QFxVisualDataModelPrivate::data(QObject *item)
{
    QList<QFxVisualDataModelData *> dataList =
        item->findChildren<QFxVisualDataModelData *>();
    Q_ASSERT(dataList.count() == 1);
    return dataList.first();
}

QFxVisualDataModel::QFxVisualDataModel()
: QFxVisualModel(*(new QFxVisualDataModelPrivate(0)))
{
}

QFxVisualDataModel::QFxVisualDataModel(QmlContext *ctxt)
: QFxVisualModel(*(new QFxVisualDataModelPrivate(ctxt)))
{
}

QFxVisualDataModel::~QFxVisualDataModel()
{
    Q_D(QFxVisualDataModel);
    if (d->m_modelList)
        delete d->m_modelList;
}

QVariant QFxVisualDataModel::model() const
{
    Q_D(const QFxVisualDataModel);
    return d->m_modelVariant;
}

void QFxVisualDataModel::setModel(const QVariant &model)
{
    Q_D(QFxVisualDataModel);
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
    if ((d->m_visualItemModel = qvariant_cast<QFxVisualDataModel *>(model))) {
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
    if (d->m_delegate && d->modelCount()) {
        emit itemsInserted(0, d->modelCount());
        emit countChanged();
    }
}

QmlComponent *QFxVisualDataModel::delegate() const
{
    Q_D(const QFxVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->delegate();
    return d->m_delegate;
}

void QFxVisualDataModel::setDelegate(QmlComponent *delegate)
{
    Q_D(QFxVisualDataModel);
    d->m_delegate = delegate;
    if (d->modelCount()) {
        emit itemsInserted(0, d->modelCount());
        emit countChanged();
    }
}

QString QFxVisualDataModel::part() const
{
    Q_D(const QFxVisualDataModel);
    return d->m_part;
}

void QFxVisualDataModel::setPart(const QString &part)
{
    Q_D(QFxVisualDataModel);
    d->m_part = part;
}

int QFxVisualDataModel::count() const
{
    Q_D(const QFxVisualDataModel);
    return d->modelCount();
}

QFxItem *QFxVisualDataModel::item(int index, bool complete)
{
    Q_D(QFxVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, d->m_part.toLatin1(), complete);
    return item(index, QByteArray(), complete);
}

/*
  Returns ReleaseStatus flags.
*/
QFxVisualDataModel::ReleaseFlags QFxVisualDataModel::release(QFxItem *item)
{
    Q_D(QFxVisualDataModel);
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

QObject *QFxVisualDataModel::parts()
{
    Q_D(QFxVisualDataModel);
    if (!d->m_parts) 
        d->m_parts = new QFxVisualDataModelParts(this);
    return d->m_parts;
}

QFxItem *QFxVisualDataModel::item(int index, const QByteArray &viewId, bool complete)
{
    Q_D(QFxVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, viewId, complete);

    if (d->modelCount() <= 0 || !d->m_delegate)
        return 0;

    QObject *nobj = d->m_cache.getItem(index);
    if (!nobj) {
        QmlContext *ccontext = d->m_context;
        if (!ccontext) ccontext = qmlContext(this);
        QmlContext *ctxt = new QmlContext(ccontext);
        QFxVisualDataModelData *data = new QFxVisualDataModelData(index, d);
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

void QFxVisualDataModel::completeItem()
{
    Q_D(QFxVisualDataModel);
    if (d->m_visualItemModel) {
        d->m_visualItemModel->completeItem();
        return;
    }

    d->m_delegate->completeCreate();
}

QVariant QFxVisualDataModel::evaluate(int index, const QString &expression, QObject *objectContext)
{
    Q_D(QFxVisualDataModel);
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
        QFxVisualDataModelData *data = new QFxVisualDataModelData(index, d);
        ctxt->addDefaultObject(data);
        QmlExpression e(ctxt, expression, objectContext);
        e.setTrackChange(false);
        value = e.value();
        delete data;
        delete ctxt;
    }

    return value;
}

int QFxVisualDataModel::indexOf(QFxItem *item, QObject *objectContext) const
{
    QmlExpression e(qmlContext(item), QLatin1String("index"), objectContext);
    e.setTrackChange(false);
    QVariant value = e.value();
    if (value.isValid())
        return value.toInt();
    return -1;
}

void QFxVisualDataModel::_q_itemsChanged(int index, int count,
                                         const QList<int> &roles)
{
    Q_D(QFxVisualDataModel);
    // XXX - highly inefficient
    for (int ii = index; ii < index + count; ++ii) {

        if (QObject *item = d->m_cache.item(ii)) {
            QFxVisualDataModelData *data = d->data(item);

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

void QFxVisualDataModel::_q_itemsInserted(int index, int count)
{
    Q_D(QFxVisualDataModel);
    // XXX - highly inefficient
    QHash<int,QFxVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int,QFxVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= index) {
            QFxVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() + count;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QFxVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsInserted(index, count);
    emit countChanged();
}

void QFxVisualDataModel::_q_itemsRemoved(int index, int count)
{
    Q_D(QFxVisualDataModel);
    // XXX - highly inefficient
    QHash<int, QFxVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int, QFxVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {
        if (iter.key() >= index && iter.key() < index + count) {
            QFxVisualDataModelPrivate::ObjectRef objRef = *iter;
            iter = d->m_cache.erase(iter);
            items.insertMulti(-1, objRef); //XXX perhaps better to maintain separately
            QFxVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(-1);
        } else if (iter.key() >= index + count) {
            QFxVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - count;
            iter = d->m_cache.erase(iter);
            items.insert(index, objRef);
            QFxVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }

    d->m_cache.unite(items);
    emit itemsRemoved(index, count);
    emit countChanged();
}

void QFxVisualDataModel::_q_itemsMoved(int from, int to, int count)
{
    Q_D(QFxVisualDataModel);
    // XXX - highly inefficient
    QHash<int,QFxVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int,QFxVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= from && iter.key() < from + count) {
            QFxVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - from + to;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QFxVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsMoved(from, to, count);
}

void QFxVisualDataModel::_q_rowsInserted(const QModelIndex &, int begin, int end)
{
    _q_itemsInserted(begin, end - begin + 1);
}

void QFxVisualDataModel::_q_rowsRemoved(const QModelIndex &, int begin, int end)
{
    _q_itemsRemoved(begin, end - begin + 1);
}

void QFxVisualDataModel::_q_dataChanged(const QModelIndex &begin, const QModelIndex &end)
{
    Q_D(QFxVisualDataModel);
    _q_itemsChanged(begin.row(), end.row() - begin.row() + 1, d->m_roles);
}

void QFxVisualDataModel::_q_createdPackage(int index, QmlPackage *package)
{
    Q_D(QFxVisualDataModel);
    emit createdItem(index, qobject_cast<QFxItem*>(package->part(d->m_part)));
}

void QFxVisualDataModel::_q_destroyingPackage(QmlPackage *package)
{
    Q_D(QFxVisualDataModel);
    emit destroyingItem(qobject_cast<QFxItem*>(package->part(d->m_part)));
}

QML_DEFINE_NOCREATE_TYPE(QFxVisualModel);
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,VisualItemModel,QFxVisualItemModel)
QML_DEFINE_TYPE(Qt,4,6,(QT_VERSION&0x00ff00)>>8,VisualDataModel,QFxVisualDataModel)

QT_END_NAMESPACE
#include "qfxvisualitemmodel.moc"
