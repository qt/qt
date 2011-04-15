// Commit: 45153a37e4d9e39e8c326a0f33ea17be49bb29e2
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qsgvisualitemmodel_p.h"
#include "qsgitem.h"

#include <QtDeclarative/qdeclarativecontext.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativeexpression.h>
#include <QtDeclarative/qdeclarativeinfo.h>

#include <private/qdeclarativecontext_p.h>
#include <private/qdeclarativepackage_p.h>
#include <private/qdeclarativeopenmetaobject_p.h>
#include <private/qdeclarativelistaccessor_p.h>
#include <private/qdeclarativedata_p.h>
#include <private/qdeclarativepropertycache_p.h>
#include <private/qdeclarativeguard_p.h>
#include <private/qdeclarativeglobal_p.h>
#include <private/qlistmodelinterface_p.h>
#include <private/qmetaobjectbuilder_p.h>
#include <private/qobject_p.h>

#include <QtCore/qhash.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

QHash<QObject*, QSGVisualItemModelAttached*> QSGVisualItemModelAttached::attachedProperties;


class QSGVisualItemModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSGVisualItemModel)
public:
    QSGVisualItemModelPrivate() : QObjectPrivate() {}

    static void children_append(QDeclarativeListProperty<QSGItem> *prop, QSGItem *item) {
        QDeclarative_setParent_noEvent(item, prop->object);
        static_cast<QSGVisualItemModelPrivate *>(prop->data)->children.append(Item(item));
        static_cast<QSGVisualItemModelPrivate *>(prop->data)->itemAppended();
        static_cast<QSGVisualItemModelPrivate *>(prop->data)->emitChildrenChanged();
    }

    static int children_count(QDeclarativeListProperty<QSGItem> *prop) {
        return static_cast<QSGVisualItemModelPrivate *>(prop->data)->children.count();
    }

    static QSGItem *children_at(QDeclarativeListProperty<QSGItem> *prop, int index) {
        return static_cast<QSGVisualItemModelPrivate *>(prop->data)->children.at(index).item;
    }

    void itemAppended() {
        Q_Q(QSGVisualItemModel);
        QSGVisualItemModelAttached *attached = QSGVisualItemModelAttached::properties(children.last().item);
        attached->setIndex(children.count()-1);
        emit q->itemsInserted(children.count()-1, 1);
        emit q->countChanged();
    }

    void emitChildrenChanged() {
        Q_Q(QSGVisualItemModel);
        emit q->childrenChanged();
    }

    int indexOf(QSGItem *item) const {
        for (int i = 0; i < children.count(); ++i)
            if (children.at(i).item == item)
                return i;
        return -1;
    }

    class Item {
    public:
        Item(QSGItem *i) : item(i), ref(0) {}

        void addRef() { ++ref; }
        bool deref() { return --ref == 0; }

        QSGItem *item;
        int ref;
    };

    QList<Item> children;
};

QSGVisualItemModel::QSGVisualItemModel(QObject *parent)
    : QSGVisualModel(*(new QSGVisualItemModelPrivate), parent)
{
}

QDeclarativeListProperty<QSGItem> QSGVisualItemModel::children()
{
    Q_D(QSGVisualItemModel);
    return QDeclarativeListProperty<QSGItem>(this, d, d->children_append,
                                                      d->children_count, d->children_at);
}

int QSGVisualItemModel::count() const
{
    Q_D(const QSGVisualItemModel);
    return d->children.count();
}

bool QSGVisualItemModel::isValid() const
{
    return true;
}

QSGItem *QSGVisualItemModel::item(int index, bool)
{
    Q_D(QSGVisualItemModel);
    QSGVisualItemModelPrivate::Item &item = d->children[index];
    item.addRef();
    return item.item;
}

QSGVisualModel::ReleaseFlags QSGVisualItemModel::release(QSGItem *item)
{
    Q_D(QSGVisualItemModel);
    int idx = d->indexOf(item);
    if (idx >= 0) {
        if (d->children[idx].deref()) {
            // XXX todo - the original did item->scene()->removeItem().  Why?
            item->setParentItem(0);
            QDeclarative_setParent_noEvent(item, this);
        }
    }
    return 0;
}

bool QSGVisualItemModel::completePending() const
{
    return false;
}

void QSGVisualItemModel::completeItem()
{
    // Nothing to do
}

QString QSGVisualItemModel::stringValue(int index, const QString &name)
{
    Q_D(QSGVisualItemModel);
    if (index < 0 || index >= d->children.count())
        return QString();
    return QDeclarativeEngine::contextForObject(d->children.at(index).item)->contextProperty(name).toString();
}

int QSGVisualItemModel::indexOf(QSGItem *item, QObject *) const
{
    Q_D(const QSGVisualItemModel);
    return d->indexOf(item);
}

QSGVisualItemModelAttached *QSGVisualItemModel::qmlAttachedProperties(QObject *obj)
{
    return QSGVisualItemModelAttached::properties(obj);
}

//============================================================================

class VDMDelegateDataType : public QDeclarativeOpenMetaObjectType
{
public:
    VDMDelegateDataType(const QMetaObject *base, QDeclarativeEngine *engine) : QDeclarativeOpenMetaObjectType(base, engine) {}

    void propertyCreated(int, QMetaPropertyBuilder &prop) {
        prop.setWritable(false);
    }
};

class QSGVisualDataModelParts;
class QSGVisualDataModelData;
class QSGVisualDataModelPrivate : public QObjectPrivate
{
public:
    QSGVisualDataModelPrivate(QDeclarativeContext *);

    static QSGVisualDataModelPrivate *get(QSGVisualDataModel *m) {
        return static_cast<QSGVisualDataModelPrivate *>(QObjectPrivate::get(m));
    }

    QDeclarativeGuard<QListModelInterface> m_listModelInterface;
    QDeclarativeGuard<QAbstractItemModel> m_abstractItemModel;
    QDeclarativeGuard<QSGVisualDataModel> m_visualItemModel;
    QString m_part;

    QDeclarativeComponent *m_delegate;
    QDeclarativeGuard<QDeclarativeContext> m_context;
    QList<int> m_roles;
    QHash<QByteArray,int> m_roleNames;
    void ensureRoles() {
        if (m_roleNames.isEmpty()) {
            if (m_listModelInterface) {
                m_roles = m_listModelInterface->roles();
                for (int ii = 0; ii < m_roles.count(); ++ii)
                    m_roleNames.insert(m_listModelInterface->toString(m_roles.at(ii)).toUtf8(), m_roles.at(ii));
            } else if (m_abstractItemModel) {
                for (QHash<int,QByteArray>::const_iterator it = m_abstractItemModel->roleNames().begin();
                        it != m_abstractItemModel->roleNames().end(); ++it) {
                    m_roles.append(it.key());
                    m_roleNames.insert(*it, it.key());
                }
                if (m_roles.count())
                    m_roleNames.insert("hasModelChildren", -1);
            } else if (m_listAccessor) {
                m_roleNames.insert("modelData", 0);
                if (m_listAccessor->type() == QDeclarativeListAccessor::Instance) {
                    if (QObject *object = m_listAccessor->at(0).value<QObject*>()) {
                        int count = object->metaObject()->propertyCount();
                        for (int ii = 1; ii < count; ++ii) {
                            const QMetaProperty &prop = object->metaObject()->property(ii);
                            m_roleNames.insert(prop.name(), 0);
                        }
                    }
                }
            }
        }
    }

    QHash<int,int> m_roleToPropId;
    int m_modelDataPropId;
    void createMetaData() {
        if (!m_metaDataCreated) {
            ensureRoles();
            if (m_roleNames.count()) {
                QHash<QByteArray, int>::const_iterator it = m_roleNames.begin();
                while (it != m_roleNames.end()) {
                    int propId = m_delegateDataType->createProperty(it.key()) - m_delegateDataType->propertyOffset();
                    m_roleToPropId.insert(*it, propId);
                    ++it;
                }
                // Add modelData property
                if (m_roles.count() == 1)
                    m_modelDataPropId = m_delegateDataType->createProperty("modelData") - m_delegateDataType->propertyOffset();
                m_metaDataCreated = true;
            }
        }
    }

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

    int modelCount() const {
        if (m_visualItemModel)
            return m_visualItemModel->count();
        if (m_listModelInterface)
            return m_listModelInterface->count();
        if (m_abstractItemModel)
            return m_abstractItemModel->rowCount(m_root);
        if (m_listAccessor)
            return m_listAccessor->count();
        return 0;
    }

    Cache m_cache;
    QHash<QObject *, QDeclarativePackage*> m_packaged;

    QSGVisualDataModelParts *m_parts;
    friend class QSGVisualItemParts;

    VDMDelegateDataType *m_delegateDataType;
    friend class QSGVisualDataModelData;
    bool m_metaDataCreated : 1;
    bool m_metaDataCacheable : 1;
    bool m_delegateValidated : 1;
    bool m_completePending : 1;

    QSGVisualDataModelData *data(QObject *item);

    QVariant m_modelVariant;
    QDeclarativeListAccessor *m_listAccessor;

    QModelIndex m_root;
    QList<QByteArray> watchedRoles;
    QList<int> watchedRoleIds;
};

class QSGVisualDataModelDataMetaObject : public QDeclarativeOpenMetaObject
{
public:
    QSGVisualDataModelDataMetaObject(QObject *parent, QDeclarativeOpenMetaObjectType *type)
    : QDeclarativeOpenMetaObject(parent, type) {}

    virtual QVariant initialValue(int);
    virtual int createProperty(const char *, const char *);

private:
    friend class QSGVisualDataModelData;
};

class QSGVisualDataModelData : public QObject
{
Q_OBJECT
public:
    QSGVisualDataModelData(int index, QSGVisualDataModel *model);
    ~QSGVisualDataModelData();

    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    int index() const;
    void setIndex(int index);

    int propForRole(int) const;
    int modelDataPropertyId() const {
        QSGVisualDataModelPrivate *model = QSGVisualDataModelPrivate::get(m_model);
        return model->m_modelDataPropId;
    }

    void setValue(int, const QVariant &);
    bool hasValue(int id) const {
        return m_meta->hasValue(id);
    }

    void ensureProperties();

Q_SIGNALS:
    void indexChanged();

private:
    friend class QSGVisualDataModelDataMetaObject;
    int m_index;
    QDeclarativeGuard<QSGVisualDataModel> m_model;
    QSGVisualDataModelDataMetaObject *m_meta;
};

int QSGVisualDataModelData::propForRole(int id) const
{
    QSGVisualDataModelPrivate *model = QSGVisualDataModelPrivate::get(m_model);
    QHash<int,int>::const_iterator it = model->m_roleToPropId.find(id);
    if (it != model->m_roleToPropId.end())
        return *it;

    return -1;
}

void QSGVisualDataModelData::setValue(int id, const QVariant &val)
{
    m_meta->setValue(id, val);
}

int QSGVisualDataModelDataMetaObject::createProperty(const char *name, const char *type)
{
    QSGVisualDataModelData *data =
        static_cast<QSGVisualDataModelData *>(object());

    if (!data->m_model)
        return -1;

    QSGVisualDataModelPrivate *model = QSGVisualDataModelPrivate::get(data->m_model);
    if (data->m_index < 0 || data->m_index >= model->modelCount())
        return -1;

    if ((!model->m_listModelInterface || !model->m_abstractItemModel) && model->m_listAccessor) {
        if (model->m_listAccessor->type() == QDeclarativeListAccessor::ListProperty) {
            model->ensureRoles();
            if (qstrcmp(name,"modelData") == 0)
                return QDeclarativeOpenMetaObject::createProperty(name, type);
        }
    }
    return -1;
}

QVariant QSGVisualDataModelDataMetaObject::initialValue(int propId)
{
    QSGVisualDataModelData *data =
        static_cast<QSGVisualDataModelData *>(object());

    Q_ASSERT(data->m_model);
    QSGVisualDataModelPrivate *model = QSGVisualDataModelPrivate::get(data->m_model);

    QByteArray propName = name(propId);
    if ((!model->m_listModelInterface || !model->m_abstractItemModel) && model->m_listAccessor) {
        if (propName == "modelData") {
            if (model->m_listAccessor->type() == QDeclarativeListAccessor::Instance) {
                QObject *object = model->m_listAccessor->at(0).value<QObject*>();
                return object->metaObject()->property(1).read(object); // the first property after objectName
            }
            return model->m_listAccessor->at(data->m_index);
        } else {
            // return any property of a single object instance.
            QObject *object = model->m_listAccessor->at(data->m_index).value<QObject*>();
            return object->property(propName);
        }
    } else if (model->m_listModelInterface) {
        model->ensureRoles();
        QHash<QByteArray,int>::const_iterator it = model->m_roleNames.find(propName);
        if (it != model->m_roleNames.end()) {
            QVariant value = model->m_listModelInterface->data(data->m_index, *it);
            return value;
        } else if (model->m_roles.count() == 1 && propName == "modelData") {
            //for compatibility with other lists, assign modelData if there is only a single role
            QVariant value = model->m_listModelInterface->data(data->m_index, model->m_roles.first());
            return value;
        }
    } else if (model->m_abstractItemModel) {
        model->ensureRoles();
        QModelIndex index = model->m_abstractItemModel->index(data->m_index, 0, model->m_root);
        if (propName == "hasModelChildren") {
            return model->m_abstractItemModel->hasChildren(index);
        } else {
            QHash<QByteArray,int>::const_iterator it = model->m_roleNames.find(propName);
            if (it != model->m_roleNames.end()) {
                return model->m_abstractItemModel->data(index, *it);
            } else if (model->m_roles.count() == 1 && propName == "modelData") {
                //for compatibility with other lists, assign modelData if there is only a single role
                return model->m_abstractItemModel->data(index, model->m_roles.first());
            }
        }
    }
    Q_ASSERT(!"Can never be reached");
    return QVariant();
}

QSGVisualDataModelData::QSGVisualDataModelData(int index,
                                               QSGVisualDataModel *model)
: m_index(index), m_model(model),
m_meta(new QSGVisualDataModelDataMetaObject(this, QSGVisualDataModelPrivate::get(model)->m_delegateDataType))
{
    ensureProperties();
}

QSGVisualDataModelData::~QSGVisualDataModelData()
{
}

void QSGVisualDataModelData::ensureProperties()
{
    QSGVisualDataModelPrivate *modelPriv = QSGVisualDataModelPrivate::get(m_model);
    if (modelPriv->m_metaDataCacheable) {
        if (!modelPriv->m_metaDataCreated)
            modelPriv->createMetaData();
        if (modelPriv->m_metaDataCreated)
            m_meta->setCached(true);
    }
}

int QSGVisualDataModelData::index() const
{
    return m_index;
}

// This is internal only - it should not be set from qml
void QSGVisualDataModelData::setIndex(int index)
{
    m_index = index;
    emit indexChanged();
}

//---------------------------------------------------------------------------

class QSGVisualDataModelPartsMetaObject : public QDeclarativeOpenMetaObject
{
public:
    QSGVisualDataModelPartsMetaObject(QObject *parent)
    : QDeclarativeOpenMetaObject(parent) {}

    virtual void propertyCreated(int, QMetaPropertyBuilder &);
    virtual QVariant initialValue(int);
};

class QSGVisualDataModelParts : public QObject
{
Q_OBJECT
public:
    QSGVisualDataModelParts(QSGVisualDataModel *parent);

private:
    friend class QSGVisualDataModelPartsMetaObject;
    QSGVisualDataModel *model;
};

void QSGVisualDataModelPartsMetaObject::propertyCreated(int, QMetaPropertyBuilder &prop)
{
    prop.setWritable(false);
}

QVariant QSGVisualDataModelPartsMetaObject::initialValue(int id)
{
    QSGVisualDataModel *m = new QSGVisualDataModel;
    m->setParent(object());
    m->setPart(QString::fromUtf8(name(id)));
    m->setModel(QVariant::fromValue(static_cast<QSGVisualDataModelParts *>(object())->model));

    QVariant var = QVariant::fromValue((QObject *)m);
    return var;
}

QSGVisualDataModelParts::QSGVisualDataModelParts(QSGVisualDataModel *parent)
: QObject(parent), model(parent)
{
    new QSGVisualDataModelPartsMetaObject(this);
}

QSGVisualDataModelPrivate::QSGVisualDataModelPrivate(QDeclarativeContext *ctxt)
: m_listModelInterface(0), m_abstractItemModel(0), m_visualItemModel(0), m_delegate(0)
, m_context(ctxt), m_modelDataPropId(-1), m_parts(0), m_delegateDataType(0), m_metaDataCreated(false)
, m_metaDataCacheable(false), m_delegateValidated(false), m_completePending(false), m_listAccessor(0)
{
}

QSGVisualDataModelData *QSGVisualDataModelPrivate::data(QObject *item)
{
    QSGVisualDataModelData *dataItem =
        item->findChild<QSGVisualDataModelData *>();
    Q_ASSERT(dataItem);
    return dataItem;
}

//---------------------------------------------------------------------------

QSGVisualDataModel::QSGVisualDataModel()
: QSGVisualModel(*(new QSGVisualDataModelPrivate(0)))
{
}

QSGVisualDataModel::QSGVisualDataModel(QDeclarativeContext *ctxt, QObject *parent)
: QSGVisualModel(*(new QSGVisualDataModelPrivate(ctxt)), parent)
{
}

QSGVisualDataModel::~QSGVisualDataModel()
{
    Q_D(QSGVisualDataModel);
    if (d->m_listAccessor)
        delete d->m_listAccessor;
    if (d->m_delegateDataType)
        d->m_delegateDataType->release();
}

QVariant QSGVisualDataModel::model() const
{
    Q_D(const QSGVisualDataModel);
    return d->m_modelVariant;
}

void QSGVisualDataModel::setModel(const QVariant &model)
{
    Q_D(QSGVisualDataModel);
    delete d->m_listAccessor;
    d->m_listAccessor = 0;
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
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                            this, SLOT(_q_rowsInserted(QModelIndex,int,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                            this, SLOT(_q_rowsRemoved(QModelIndex,int,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                            this, SLOT(_q_dataChanged(QModelIndex,QModelIndex)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                            this, SLOT(_q_rowsMoved(QModelIndex,int,int,QModelIndex,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(modelReset()), this, SLOT(_q_modelReset()));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(layoutChanged()), this, SLOT(_q_layoutChanged()));
        d->m_abstractItemModel = 0;
    } else if (d->m_visualItemModel) {
        QObject::disconnect(d->m_visualItemModel, SIGNAL(itemsInserted(int,int)),
                         this, SIGNAL(itemsInserted(int,int)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(itemsRemoved(int,int)),
                         this, SIGNAL(itemsRemoved(int,int)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(itemsMoved(int,int,int)),
                         this, SIGNAL(itemsMoved(int,int,int)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(createdPackage(int,QDeclarativePackage*)),
                         this, SLOT(_q_createdPackage(int,QDeclarativePackage*)));
        QObject::disconnect(d->m_visualItemModel, SIGNAL(destroyingPackage(QDeclarativePackage*)),
                         this, SLOT(_q_destroyingPackage(QDeclarativePackage*)));
        d->m_visualItemModel = 0;
    }

    d->m_roles.clear();
    d->m_roleNames.clear();
    if (d->m_delegateDataType)
        d->m_delegateDataType->release();
    d->m_metaDataCreated = 0;
    d->m_metaDataCacheable = false;
    d->m_delegateDataType = new VDMDelegateDataType(&QSGVisualDataModelData::staticMetaObject, d->m_context?d->m_context->engine():qmlEngine(this));

    QObject *object = qvariant_cast<QObject *>(model);
    if (object && (d->m_listModelInterface = qobject_cast<QListModelInterface *>(object))) {
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsChanged(int,int,QList<int>)),
                         this, SLOT(_q_itemsChanged(int,int,QList<int>)));
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsInserted(int,int)),
                         this, SLOT(_q_itemsInserted(int,int)));
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsRemoved(int,int)),
                         this, SLOT(_q_itemsRemoved(int,int)));
        QObject::connect(d->m_listModelInterface, SIGNAL(itemsMoved(int,int,int)),
                         this, SLOT(_q_itemsMoved(int,int,int)));
        d->m_metaDataCacheable = true;
        if (d->m_delegate && d->m_listModelInterface->count())
            emit itemsInserted(0, d->m_listModelInterface->count());
        return;
    } else if (object && (d->m_abstractItemModel = qobject_cast<QAbstractItemModel *>(object))) {
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
                            this, SLOT(_q_rowsInserted(QModelIndex,int,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsRemoved(QModelIndex,int,int)),
                            this, SLOT(_q_rowsRemoved(QModelIndex,int,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                            this, SLOT(_q_dataChanged(QModelIndex,QModelIndex)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)),
                            this, SLOT(_q_rowsMoved(QModelIndex,int,int,QModelIndex,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(modelReset()), this, SLOT(_q_modelReset()));
        QObject::connect(d->m_abstractItemModel, SIGNAL(layoutChanged()), this, SLOT(_q_layoutChanged()));
        d->m_metaDataCacheable = true;
        if (d->m_abstractItemModel->canFetchMore(d->m_root))
            d->m_abstractItemModel->fetchMore(d->m_root);
        return;
    }
    if ((d->m_visualItemModel = qvariant_cast<QSGVisualDataModel *>(model))) {
        QObject::connect(d->m_visualItemModel, SIGNAL(itemsInserted(int,int)),
                         this, SIGNAL(itemsInserted(int,int)));
        QObject::connect(d->m_visualItemModel, SIGNAL(itemsRemoved(int,int)),
                         this, SIGNAL(itemsRemoved(int,int)));
        QObject::connect(d->m_visualItemModel, SIGNAL(itemsMoved(int,int,int)),
                         this, SIGNAL(itemsMoved(int,int,int)));
        QObject::connect(d->m_visualItemModel, SIGNAL(createdPackage(int,QDeclarativePackage*)),
                         this, SLOT(_q_createdPackage(int,QDeclarativePackage*)));
        QObject::connect(d->m_visualItemModel, SIGNAL(destroyingPackage(QDeclarativePackage*)),
                         this, SLOT(_q_destroyingPackage(QDeclarativePackage*)));
        return;
    }
    d->m_listAccessor = new QDeclarativeListAccessor;
    d->m_listAccessor->setList(model, d->m_context?d->m_context->engine():qmlEngine(this));
    if (d->m_listAccessor->type() != QDeclarativeListAccessor::ListProperty)
        d->m_metaDataCacheable = true;
    if (d->m_delegate && d->modelCount()) {
        emit itemsInserted(0, d->modelCount());
        emit countChanged();
    }
}

QDeclarativeComponent *QSGVisualDataModel::delegate() const
{
    Q_D(const QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->delegate();
    return d->m_delegate;
}

void QSGVisualDataModel::setDelegate(QDeclarativeComponent *delegate)
{
    Q_D(QSGVisualDataModel);
    bool wasValid = d->m_delegate != 0;
    d->m_delegate = delegate;
    d->m_delegateValidated = false;
    if (!wasValid && d->modelCount() && d->m_delegate) {
        emit itemsInserted(0, d->modelCount());
        emit countChanged();
    }
    if (wasValid && !d->m_delegate && d->modelCount()) {
        emit itemsRemoved(0, d->modelCount());
        emit countChanged();
    }
}

QVariant QSGVisualDataModel::rootIndex() const
{
    Q_D(const QSGVisualDataModel);
    return QVariant::fromValue(d->m_root);
}

void QSGVisualDataModel::setRootIndex(const QVariant &root)
{
    Q_D(QSGVisualDataModel);
    QModelIndex modelIndex = qvariant_cast<QModelIndex>(root);
    if (d->m_root != modelIndex) {
        int oldCount = d->modelCount();
        d->m_root = modelIndex;
        if (d->m_abstractItemModel && d->m_abstractItemModel->canFetchMore(modelIndex))
            d->m_abstractItemModel->fetchMore(modelIndex);
        int newCount = d->modelCount();
        if (d->m_delegate && oldCount)
            emit itemsRemoved(0, oldCount);
        if (d->m_delegate && newCount)
            emit itemsInserted(0, newCount);
        if (newCount != oldCount)
            emit countChanged();
        emit rootIndexChanged();
    }
}

QVariant QSGVisualDataModel::modelIndex(int idx) const
{
    Q_D(const QSGVisualDataModel);
    if (d->m_abstractItemModel)
        return QVariant::fromValue(d->m_abstractItemModel->index(idx, 0, d->m_root));
    return QVariant::fromValue(QModelIndex());
}

QVariant QSGVisualDataModel::parentModelIndex() const
{
    Q_D(const QSGVisualDataModel);
    if (d->m_abstractItemModel)
        return QVariant::fromValue(d->m_abstractItemModel->parent(d->m_root));
    return QVariant::fromValue(QModelIndex());
}

QString QSGVisualDataModel::part() const
{
    Q_D(const QSGVisualDataModel);
    return d->m_part;
}

void QSGVisualDataModel::setPart(const QString &part)
{
    Q_D(QSGVisualDataModel);
    d->m_part = part;
}

int QSGVisualDataModel::count() const
{
    Q_D(const QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->count();
    if (!d->m_delegate)
        return 0;
    return d->modelCount();
}

QSGItem *QSGVisualDataModel::item(int index, bool complete)
{
    Q_D(QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, d->m_part.toUtf8(), complete);
    return item(index, QByteArray(), complete);
}

/*
  Returns ReleaseStatus flags.
*/
QSGVisualDataModel::ReleaseFlags QSGVisualDataModel::release(QSGItem *item)
{
    Q_D(QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->release(item);

    ReleaseFlags stat = 0;
    QObject *obj = item;
    bool inPackage = false;

    QHash<QObject*,QDeclarativePackage*>::iterator it = d->m_packaged.find(item);
    if (it != d->m_packaged.end()) {
        QDeclarativePackage *package = *it;
        d->m_packaged.erase(it);
        if (d->m_packaged.contains(item))
            stat |= Referenced;
        inPackage = true;
        obj = package; // fall through and delete
    }

    if (d->m_cache.releaseItem(obj)) {
        // Remove any bindings to avoid warnings due to parent change.
        QObjectPrivate *p = QObjectPrivate::get(obj);
        Q_ASSERT(p->declarativeData);
        QDeclarativeData *d = static_cast<QDeclarativeData*>(p->declarativeData);
        if (d->ownContext && d->context)
            d->context->clearContext();

        if (inPackage) {
            emit destroyingPackage(qobject_cast<QDeclarativePackage*>(obj));
        } else {
            // XXX todo - the original did item->scene()->removeItem().  Why?
            item->setParentItem(0);
        }
        stat |= Destroyed;
        obj->deleteLater();
    } else if (!inPackage) {
        stat |= Referenced;
    }

    return stat;
}

QObject *QSGVisualDataModel::parts()
{
    Q_D(QSGVisualDataModel);
    if (!d->m_parts)
        d->m_parts = new QSGVisualDataModelParts(this);
    return d->m_parts;
}

QSGItem *QSGVisualDataModel::item(int index, const QByteArray &viewId, bool complete)
{
    Q_D(QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, viewId, complete);

    if (d->modelCount() <= 0 || !d->m_delegate)
        return 0;
    QObject *nobj = d->m_cache.getItem(index);
    bool needComplete = false;
    if (!nobj) {
        QDeclarativeContext *ccontext = d->m_context;
        if (!ccontext) ccontext = qmlContext(this);
        QDeclarativeContext *ctxt = new QDeclarativeContext(ccontext);
        QSGVisualDataModelData *data = new QSGVisualDataModelData(index, this);
        if ((!d->m_listModelInterface || !d->m_abstractItemModel) && d->m_listAccessor
            && d->m_listAccessor->type() == QDeclarativeListAccessor::ListProperty) {
            ctxt->setContextObject(d->m_listAccessor->at(index).value<QObject*>());
            ctxt = new QDeclarativeContext(ctxt, ctxt);
        }
        ctxt->setContextProperty(QLatin1String("model"), data);
        ctxt->setContextObject(data);
        d->m_completePending = false;
        nobj = d->m_delegate->beginCreate(ctxt);
        if (complete) {
            d->m_delegate->completeCreate();
        } else {
            d->m_completePending = true;
            needComplete = true;
        }
        if (nobj) {
            QDeclarative_setParent_noEvent(ctxt, nobj);
            QDeclarative_setParent_noEvent(data, nobj);
            d->m_cache.insertItem(index, nobj);
            if (QDeclarativePackage *package = qobject_cast<QDeclarativePackage *>(nobj))
                emit createdPackage(index, package);
        } else {
            delete data;
            delete ctxt;
            qmlInfo(this, d->m_delegate->errors()) << "Error creating delegate";
        }
    }
    QSGItem *item = qobject_cast<QSGItem *>(nobj);
    if (!item) {
        QDeclarativePackage *package = qobject_cast<QDeclarativePackage *>(nobj);
        if (package) {
            QObject *o = package->part(QString::fromUtf8(viewId));
            item = qobject_cast<QSGItem *>(o);
            if (item)
                d->m_packaged.insertMulti(item, package);
        }
    }
    if (!item) {
        if (needComplete)
            d->m_delegate->completeCreate();
        d->m_cache.releaseItem(nobj);
        if (!d->m_delegateValidated) {
            qmlInfo(d->m_delegate) << QSGVisualDataModel::tr("Delegate component must be Item type.");
            d->m_delegateValidated = true;
        }
    }
    if (d->modelCount()-1 == index && d->m_abstractItemModel && d->m_abstractItemModel->canFetchMore(d->m_root))
        d->m_abstractItemModel->fetchMore(d->m_root);

    return item;
}

bool QSGVisualDataModel::completePending() const
{
    Q_D(const QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->completePending();
    return d->m_completePending;
}

void QSGVisualDataModel::completeItem()
{
    Q_D(QSGVisualDataModel);
    if (d->m_visualItemModel) {
        d->m_visualItemModel->completeItem();
        return;
    }

    d->m_delegate->completeCreate();
    d->m_completePending = false;
}

QString QSGVisualDataModel::stringValue(int index, const QString &name)
{
    Q_D(QSGVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->stringValue(index, name);

    if ((!d->m_listModelInterface || !d->m_abstractItemModel) && d->m_listAccessor) {
        if (QObject *object = d->m_listAccessor->at(index).value<QObject*>())
            return object->property(name.toUtf8()).toString();
    }

    if ((!d->m_listModelInterface && !d->m_abstractItemModel) || !d->m_delegate)
        return QString();

    QString val;
    QObject *data = 0;
    bool tempData = false;

    if (QObject *nobj = d->m_cache.item(index))
        data = d->data(nobj);
    if (!data) {
        data = new QSGVisualDataModelData(index, this);
        tempData = true;
    }

    QDeclarativeData *ddata = QDeclarativeData::get(data);
    if (ddata && ddata->propertyCache) {
        QDeclarativePropertyCache::Data *prop = ddata->propertyCache->property(name);
        if (prop) {
            if (prop->propType == QVariant::String) {
                void *args[] = { &val, 0 };
                QMetaObject::metacall(data, QMetaObject::ReadProperty, prop->coreIndex, args);
            } else if (prop->propType == qMetaTypeId<QVariant>()) {
                QVariant v;
                void *args[] = { &v, 0 };
                QMetaObject::metacall(data, QMetaObject::ReadProperty, prop->coreIndex, args);
                val = v.toString();
            }
        } else {
            val = data->property(name.toUtf8()).toString();
        }
    } else {
        val = data->property(name.toUtf8()).toString();
    }

    if (tempData)
        delete data;

    return val;
}

int QSGVisualDataModel::indexOf(QSGItem *item, QObject *) const
{
    QVariant val = QDeclarativeEngine::contextForObject(item)->contextProperty(QLatin1String("index"));
        return val.toInt();
    return -1;
}

void QSGVisualDataModel::setWatchedRoles(QList<QByteArray> roles)
{
    Q_D(QSGVisualDataModel);
    d->watchedRoles = roles;
    d->watchedRoleIds.clear();
}

void QSGVisualDataModel::_q_itemsChanged(int index, int count,
                                         const QList<int> &roles)
{
    Q_D(QSGVisualDataModel);
    bool changed = false;
    if (!d->watchedRoles.isEmpty() && d->watchedRoleIds.isEmpty()) {
        foreach (QByteArray r, d->watchedRoles) {
            if (d->m_roleNames.contains(r))
                d->watchedRoleIds << d->m_roleNames.value(r);
        }
    }

    for (QHash<int,QSGVisualDataModelPrivate::ObjectRef>::ConstIterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ++iter) {
        const int idx = iter.key();

        if (idx >= index && idx < index+count) {
            QSGVisualDataModelPrivate::ObjectRef objRef = *iter;
            QSGVisualDataModelData *data = d->data(objRef.obj);
            for (int roleIdx = 0; roleIdx < roles.count(); ++roleIdx) {
                int role = roles.at(roleIdx);
                if (!changed && !d->watchedRoleIds.isEmpty() && d->watchedRoleIds.contains(role))
                    changed = true;
                int propId = data->propForRole(role);
                if (propId != -1) {
                    if (data->hasValue(propId)) {
                        if (d->m_listModelInterface) {
                            data->setValue(propId, d->m_listModelInterface->data(idx, role));
                        } else if (d->m_abstractItemModel) {
                            QModelIndex index = d->m_abstractItemModel->index(idx, 0, d->m_root);
                            data->setValue(propId, d->m_abstractItemModel->data(index, role));
                        }
                    }
                } else {
                    QString roleName;
                    if (d->m_listModelInterface)
                        roleName = d->m_listModelInterface->toString(role);
                    else if (d->m_abstractItemModel)
                        roleName = QString::fromUtf8(d->m_abstractItemModel->roleNames().value(role));
                    qmlInfo(this) << "Changing role not present in item: " << roleName;
                }
            }
            if (d->m_roles.count() == 1) {
                // Handle the modelData role we add if there is just one role.
                int propId = data->modelDataPropertyId();
                if (data->hasValue(propId)) {
                    int role = d->m_roles.at(0);
                    if (d->m_listModelInterface) {
                        data->setValue(propId, d->m_listModelInterface->data(idx, role));
                    } else if (d->m_abstractItemModel) {
                        QModelIndex index = d->m_abstractItemModel->index(idx, 0, d->m_root);
                        data->setValue(propId, d->m_abstractItemModel->data(index, role));
                    }
                }
            }
        }
    }
    if (changed)
        emit itemsChanged(index, count);
}

void QSGVisualDataModel::_q_itemsInserted(int index, int count)
{
    Q_D(QSGVisualDataModel);
    if (!count)
        return;
    // XXX - highly inefficient
    QHash<int,QSGVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int,QSGVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= index) {
            QSGVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() + count;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QSGVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsInserted(index, count);
    emit countChanged();
}

void QSGVisualDataModel::_q_itemsRemoved(int index, int count)
{
    Q_D(QSGVisualDataModel);
    if (!count)
        return;
    // XXX - highly inefficient
    QHash<int, QSGVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int, QSGVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {
        if (iter.key() >= index && iter.key() < index + count) {
            QSGVisualDataModelPrivate::ObjectRef objRef = *iter;
            iter = d->m_cache.erase(iter);
            items.insertMulti(-1, objRef); //XXX perhaps better to maintain separately
            QSGVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(-1);
        } else if (iter.key() >= index + count) {
            QSGVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - count;
            iter = d->m_cache.erase(iter);
            items.insert(index, objRef);
            QSGVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }

    d->m_cache.unite(items);
    emit itemsRemoved(index, count);
    emit countChanged();
}

void QSGVisualDataModel::_q_itemsMoved(int from, int to, int count)
{
    Q_D(QSGVisualDataModel);
    // XXX - highly inefficient
    QHash<int,QSGVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int,QSGVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= from && iter.key() < from + count) {
            QSGVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - from + to;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QSGVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    for (QHash<int,QSGVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        int diff = from > to ? count : -count;
        if (iter.key() >= qMin(from,to) && iter.key() < qMax(from+count,to+count)) {
            QSGVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() + diff;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QSGVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsMoved(from, to, count);
}

void QSGVisualDataModel::_q_rowsInserted(const QModelIndex &parent, int begin, int end)
{
    Q_D(QSGVisualDataModel);
    if (parent == d->m_root)
        _q_itemsInserted(begin, end - begin + 1);
}

void QSGVisualDataModel::_q_rowsRemoved(const QModelIndex &parent, int begin, int end)
{
    Q_D(QSGVisualDataModel);
    if (parent == d->m_root)
        _q_itemsRemoved(begin, end - begin + 1);
}

void QSGVisualDataModel::_q_rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    Q_D(QSGVisualDataModel);
    const int count = sourceEnd - sourceStart + 1;
    if (destinationParent == d->m_root && sourceParent == d->m_root) {
        _q_itemsMoved(sourceStart, sourceStart > destinationRow ? destinationRow : destinationRow-1, count);
    } else if (sourceParent == d->m_root) {
        _q_itemsRemoved(sourceStart, count);
    } else if (destinationParent == d->m_root) {
        _q_itemsInserted(destinationRow, count);
    }
}

void QSGVisualDataModel::_q_dataChanged(const QModelIndex &begin, const QModelIndex &end)
{
    Q_D(QSGVisualDataModel);
    if (begin.parent() == d->m_root)
        _q_itemsChanged(begin.row(), end.row() - begin.row() + 1, d->m_roles);
}

void QSGVisualDataModel::_q_layoutChanged()
{
    Q_D(QSGVisualDataModel);
    _q_itemsChanged(0, count(), d->m_roles);
}

void QSGVisualDataModel::_q_modelReset()
{
    emit modelReset();
}

void QSGVisualDataModel::_q_createdPackage(int index, QDeclarativePackage *package)
{
    Q_D(QSGVisualDataModel);
    emit createdItem(index, qobject_cast<QSGItem*>(package->part(d->m_part)));
}

void QSGVisualDataModel::_q_destroyingPackage(QDeclarativePackage *package)
{
    Q_D(QSGVisualDataModel);
    emit destroyingItem(qobject_cast<QSGItem*>(package->part(d->m_part)));
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QListModelInterface)

#include <qsgvisualitemmodel.moc>
