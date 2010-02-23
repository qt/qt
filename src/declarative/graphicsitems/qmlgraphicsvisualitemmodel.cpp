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

#include "qmlgraphicsvisualitemmodel_p.h"

#include "qmlgraphicsitem.h"

#include <qmlcontext.h>
#include <qmlengine.h>
#include <qmlexpression.h>
#include <qmlpackage_p.h>
#include <qmlopenmetaobject_p.h>
#include <qmllistaccessor_p.h>
#include <qmlinfo.h>
#include <qmldeclarativedata_p.h>
#include <qmlpropertycache_p.h>
#include <qmlguard_p.h>

#include <qlistmodelinterface_p.h>
#include <qhash.h>
#include <qlist.h>
#include <qmetaobjectbuilder_p.h>
#include <QtCore/qdebug.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

QHash<QObject*, QmlGraphicsVisualItemModelAttached*> QmlGraphicsVisualItemModelAttached::attachedProperties;


class QmlGraphicsVisualItemModelPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QmlGraphicsVisualItemModel)
public:
    QmlGraphicsVisualItemModelPrivate() : QObjectPrivate() {}

    static void children_append(QmlListProperty<QmlGraphicsItem> *prop, QmlGraphicsItem *item) {
        item->QObject::setParent(prop->object);
        static_cast<QmlGraphicsVisualItemModelPrivate *>(prop->data)->children.append(item);
        static_cast<QmlGraphicsVisualItemModelPrivate *>(prop->data)->itemAppended();
        static_cast<QmlGraphicsVisualItemModelPrivate *>(prop->data)->emitChildrenChanged();
    }

    void itemAppended() {
        Q_Q(QmlGraphicsVisualItemModel);
        QmlGraphicsVisualItemModelAttached *attached = QmlGraphicsVisualItemModelAttached::properties(children.last());
        attached->setIndex(children.count()-1);
        emit q->itemsInserted(children.count()-1, 1);
        emit q->countChanged();
    }

    void emitChildrenChanged() {
        Q_Q(QmlGraphicsVisualItemModel);
        emit q->childrenChanged();
    }

    QList<QmlGraphicsItem *> children;
};


/*!
    \qmlclass VisualItemModel QmlGraphicsVisualItemModel
    \brief The VisualItemModel allows items to be provided to a view.

    The children of the VisualItemModel are provided in a model which
    can be used in a view.  Note that no delegate should be
    provided to a view since the VisualItemModel contains the
    visual delegate (items).

    An item can determine its index within the
    model via the VisualItemModel.index attached property.

    The example below places three colored rectangles in a ListView.
    \code
    Item {
        VisualItemModel {
            id: itemModel
            Rectangle { height: 30; width: 80; color: "red" }
            Rectangle { height: 30; width: 80; color: "green" }
            Rectangle { height: 30; width: 80; color: "blue" }
        }

        ListView {
            anchors.fill: parent
            model: itemModel
        }
    }
    \endcode
*/
QmlGraphicsVisualItemModel::QmlGraphicsVisualItemModel()
    : QmlGraphicsVisualModel(*(new QmlGraphicsVisualItemModelPrivate))
{
}

QmlListProperty<QmlGraphicsItem> QmlGraphicsVisualItemModel::children()
{
    Q_D(QmlGraphicsVisualItemModel);
    return QmlListProperty<QmlGraphicsItem>(this, d, QmlGraphicsVisualItemModelPrivate::children_append);
}

/*!
    \qmlproperty int VisualItemModel::count

    The number of items in the model.  This property is readonly.
*/
int QmlGraphicsVisualItemModel::count() const
{
    Q_D(const QmlGraphicsVisualItemModel);
    return d->children.count();
}

bool QmlGraphicsVisualItemModel::isValid() const
{
    return true;
}

QmlGraphicsItem *QmlGraphicsVisualItemModel::item(int index, bool)
{
    Q_D(QmlGraphicsVisualItemModel);
    return d->children.at(index);
}

QmlGraphicsVisualModel::ReleaseFlags QmlGraphicsVisualItemModel::release(QmlGraphicsItem *)
{
    // Nothing to do
    return 0;
}

void QmlGraphicsVisualItemModel::completeItem()
{
    // Nothing to do
}

QString QmlGraphicsVisualItemModel::stringValue(int index, const QString &name)
{
    Q_D(QmlGraphicsVisualItemModel);
    if (index < 0 || index >= d->children.count())
        return QString();
    return QmlEngine::contextForObject(d->children.at(index))->contextProperty(name).toString();
}

QVariant QmlGraphicsVisualItemModel::evaluate(int index, const QString &expression, QObject *objectContext)
{
    Q_D(QmlGraphicsVisualItemModel);
    if (index < 0 || index >= d->children.count())
        return QVariant();
    QmlContext *ccontext = qmlContext(this);
    QmlContext *ctxt = new QmlContext(ccontext);
    ctxt->addDefaultObject(d->children.at(index));
    QmlExpression e(ctxt, expression, objectContext);
    e.setTrackChange(false);
    QVariant value = e.value();
    delete ctxt;
    return value;
}

int QmlGraphicsVisualItemModel::indexOf(QmlGraphicsItem *item, QObject *) const
{
    Q_D(const QmlGraphicsVisualItemModel);
    return d->children.indexOf(item);
}

QmlGraphicsVisualItemModelAttached *QmlGraphicsVisualItemModel::qmlAttachedProperties(QObject *obj)
{
    return QmlGraphicsVisualItemModelAttached::properties(obj);
}

//============================================================================

class VDMDelegateDataType : public QmlOpenMetaObjectType
{
public:
    VDMDelegateDataType(const QMetaObject *base, QmlEngine *engine) : QmlOpenMetaObjectType(base, engine) {}

    void propertyCreated(int, QMetaPropertyBuilder &prop) {
        prop.setWritable(false);
    }
};

class QmlGraphicsVisualDataModelParts;
class QmlGraphicsVisualDataModelData;
class QmlGraphicsVisualDataModelPrivate : public QObjectPrivate
{
public:
    QmlGraphicsVisualDataModelPrivate(QmlContext *);

    static QmlGraphicsVisualDataModelPrivate *get(QmlGraphicsVisualDataModel *m) {
        return static_cast<QmlGraphicsVisualDataModelPrivate *>(QObjectPrivate::get(m));
    }

    QmlGuard<QListModelInterface> m_listModelInterface;
    QmlGuard<QAbstractItemModel> m_abstractItemModel;
    QmlGuard<QmlGraphicsVisualDataModel> m_visualItemModel;
    QString m_part;

    QmlComponent *m_delegate;
    QmlContext *m_context;
    QList<int> m_roles;
    QHash<QByteArray,int> m_roleNames;
    void ensureRoles() {
        if (m_roleNames.isEmpty()) {
            if (m_listModelInterface) {
                m_roles = m_listModelInterface->roles();
                for (int ii = 0; ii < m_roles.count(); ++ii)
                    m_roleNames.insert(m_listModelInterface->toString(m_roles.at(ii)).toUtf8(), m_roles.at(ii));
                if (m_roles.count() == 1)
                    m_roleNames.insert("modelData", m_roles.at(0));
            } else if (m_abstractItemModel) {
                for (QHash<int,QByteArray>::const_iterator it = m_abstractItemModel->roleNames().begin();
                        it != m_abstractItemModel->roleNames().end(); ++it) {
                    m_roles.append(it.key());
                    m_roleNames.insert(*it, it.key());
                }
                if (m_roles.count() == 1)
                    m_roleNames.insert("modelData", m_roles.at(0));
            } else if (m_listAccessor) {
                m_roleNames.insert("modelData", 0);
                if (m_listAccessor->type() == QmlListAccessor::Instance) {
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

    void createMetaData() {
        if (!m_metaDataCreated) {
            ensureRoles();
            QHash<QByteArray, int>::const_iterator it = m_roleNames.begin();
            while (it != m_roleNames.end()) {
                m_delegateDataType->createProperty(it.key());
                ++it;
            }
            m_metaDataCreated = true;
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
    QHash<QObject *, QmlPackage*> m_packaged;

    QmlGraphicsVisualDataModelParts *m_parts;
    friend class QmlGraphicsVisualItemParts;

    VDMDelegateDataType *m_delegateDataType;
    friend class QmlGraphicsVisualDataModelData;
    bool m_metaDataCreated;
    bool m_metaDataCacheable;

    QmlGraphicsVisualDataModelData *data(QObject *item);

    QVariant m_modelVariant;
    QmlListAccessor *m_listAccessor;

    QModelIndex m_root;
};

class QmlGraphicsVisualDataModelDataMetaObject : public QmlOpenMetaObject
{
public:
    QmlGraphicsVisualDataModelDataMetaObject(QObject *parent, QmlOpenMetaObjectType *type)
    : QmlOpenMetaObject(parent, type) {}

    virtual QVariant initialValue(int);
    virtual int createProperty(const char *, const char *);

private:
    friend class QmlGraphicsVisualDataModelData;
    QHash<int,int> roleToProp;
};

class QmlGraphicsVisualDataModelData : public QObject
{
Q_OBJECT
public:
    QmlGraphicsVisualDataModelData(int index, QmlGraphicsVisualDataModel *model);
    ~QmlGraphicsVisualDataModelData();

    Q_PROPERTY(int index READ index NOTIFY indexChanged)
    int index() const;
    void setIndex(int index);

    int propForRole(int) const;
    void setValue(int, const QVariant &);

Q_SIGNALS:
    void indexChanged();

private:
    friend class QmlGraphicsVisualDataModelDataMetaObject;
    int m_index;
    QmlGuard<QmlGraphicsVisualDataModel> m_model;
    QmlGraphicsVisualDataModelDataMetaObject *m_meta;
};

int QmlGraphicsVisualDataModelData::propForRole(int id) const
{
    QHash<int,int>::const_iterator it = m_meta->roleToProp.find(id);
    if (it != m_meta->roleToProp.end())
        return m_meta->roleToProp[id];
    return -1;
}

void QmlGraphicsVisualDataModelData::setValue(int id, const QVariant &val)
{
    m_meta->setValue(id, val);
}

int QmlGraphicsVisualDataModelDataMetaObject::createProperty(const char *name, const char *type)
{
    QmlGraphicsVisualDataModelData *data =
        static_cast<QmlGraphicsVisualDataModelData *>(object());

    if (!data->m_model)
        return -1;

    QmlGraphicsVisualDataModelPrivate *model = QmlGraphicsVisualDataModelPrivate::get(data->m_model);

    if ((!model->m_listModelInterface || !model->m_abstractItemModel) && model->m_listAccessor) {
        if (model->m_listAccessor->type() == QmlListAccessor::ListProperty) {
            model->ensureRoles();
            QObject *object = model->m_listAccessor->at(data->m_index).value<QObject*>();
            if (object && object->property(name).isValid())
                return QmlOpenMetaObject::createProperty(name, type);
        }
    }
    return -1;
}

QVariant QmlGraphicsVisualDataModelDataMetaObject::initialValue(int propId)
{
    QmlGraphicsVisualDataModelData *data =
        static_cast<QmlGraphicsVisualDataModelData *>(object());

    Q_ASSERT(data->m_model);
    QmlGraphicsVisualDataModelPrivate *model = QmlGraphicsVisualDataModelPrivate::get(data->m_model);

    QByteArray propName = name(propId);
    if ((!model->m_listModelInterface || !model->m_abstractItemModel) && model->m_listAccessor) {
        if (propName == "modelData") {
            if (model->m_listAccessor->type() == QmlListAccessor::Instance) {
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
            roleToProp.insert(*it, propId);
            QVariant value = model->m_listModelInterface->data(data->m_index, *it);
            return value;
        } else if (model->m_roles.count() == 1 && propName == "modelData") {
            //for compatability with other lists, assign modelData if there is only a single role
            roleToProp.insert(model->m_roles.first(), propId);
            QVariant value = model->m_listModelInterface->data(data->m_index, model->m_roles.first());
            return value;
        }
    } else if (model->m_abstractItemModel) {
        model->ensureRoles();
        QHash<QByteArray,int>::const_iterator it = model->m_roleNames.find(propName);
        if (it != model->m_roleNames.end()) {
            roleToProp.insert(*it, propId);
            QModelIndex index = model->m_abstractItemModel->index(data->m_index, 0, model->m_root);
            return model->m_abstractItemModel->data(index, *it);
        }
    }
    Q_ASSERT(!"Can never be reached");
    return QVariant();
}

QmlGraphicsVisualDataModelData::QmlGraphicsVisualDataModelData(int index,
                                               QmlGraphicsVisualDataModel *model)
: m_index(index), m_model(model), 
m_meta(new QmlGraphicsVisualDataModelDataMetaObject(this, QmlGraphicsVisualDataModelPrivate::get(model)->m_delegateDataType))
{
    QmlGraphicsVisualDataModelPrivate *modelPriv = QmlGraphicsVisualDataModelPrivate::get(model);
    if (modelPriv->m_metaDataCacheable) {
        if (!modelPriv->m_metaDataCreated)
            modelPriv->createMetaData();
        m_meta->setCached(true);
    }
}

QmlGraphicsVisualDataModelData::~QmlGraphicsVisualDataModelData()
{
}

int QmlGraphicsVisualDataModelData::index() const
{
    return m_index;
}

// This is internal only - it should not be set from qml
void QmlGraphicsVisualDataModelData::setIndex(int index)
{
    m_index = index;
    emit indexChanged();
}

//---------------------------------------------------------------------------

class QmlGraphicsVisualDataModelPartsMetaObject : public QmlOpenMetaObject
{
public:
    QmlGraphicsVisualDataModelPartsMetaObject(QObject *parent)
    : QmlOpenMetaObject(parent) {}

    virtual void propertyCreated(int, QMetaPropertyBuilder &);
    virtual QVariant initialValue(int);
};

class QmlGraphicsVisualDataModelParts : public QObject
{
Q_OBJECT
public:
    QmlGraphicsVisualDataModelParts(QmlGraphicsVisualDataModel *parent);

private:
    friend class QmlGraphicsVisualDataModelPartsMetaObject;
    QmlGraphicsVisualDataModel *model;
};

void QmlGraphicsVisualDataModelPartsMetaObject::propertyCreated(int, QMetaPropertyBuilder &prop)
{
    prop.setWritable(false);
}

QVariant QmlGraphicsVisualDataModelPartsMetaObject::initialValue(int id)
{
    QmlGraphicsVisualDataModel *m = new QmlGraphicsVisualDataModel;
    m->setParent(object());
    m->setPart(QString::fromUtf8(name(id)));
    m->setModel(QVariant::fromValue(static_cast<QmlGraphicsVisualDataModelParts *>(object())->model));

    QVariant var = QVariant::fromValue((QObject *)m);
    return var;
}

QmlGraphicsVisualDataModelParts::QmlGraphicsVisualDataModelParts(QmlGraphicsVisualDataModel *parent)
: QObject(parent), model(parent) 
{
    new QmlGraphicsVisualDataModelPartsMetaObject(this);
}

QmlGraphicsVisualDataModelPrivate::QmlGraphicsVisualDataModelPrivate(QmlContext *ctxt)
: m_listModelInterface(0), m_abstractItemModel(0), m_visualItemModel(0), m_delegate(0)
, m_context(ctxt), m_parts(0), m_delegateDataType(0), m_metaDataCreated(false)
, m_metaDataCacheable(false), m_listAccessor(0)
{
}

QmlGraphicsVisualDataModelData *QmlGraphicsVisualDataModelPrivate::data(QObject *item)
{
    QmlGraphicsVisualDataModelData *dataItem =
        item->findChild<QmlGraphicsVisualDataModelData *>();
    Q_ASSERT(dataItem);
    return dataItem;
}

//---------------------------------------------------------------------------

/*!
    \qmlclass VisualDataModel QmlGraphicsVisualDataModel
    \brief The VisualDataModel encapsulates a model and delegate

    A VisualDataModel encapsulates a model and the delegate that will
    be instantiated for items in the model.

    It is usually not necessary to create a VisualDataModel directly,
    since the QML views will create one internally.

    The example below illustrates using a VisualDataModel with a ListView.

    \code
    VisualDataModel {
        id: visualModel
        model: myModel
        delegate: Component {
            Rectangle {
                height: 25
                width: 100
                Text { text: "Name:" + name}
            }
        }
    }
    ListView {
        width: 100
        height: 100
        anchors.fill: parent
        model: visualModel
    }
    \endcode
*/

QmlGraphicsVisualDataModel::QmlGraphicsVisualDataModel()
: QmlGraphicsVisualModel(*(new QmlGraphicsVisualDataModelPrivate(0)))
{
}

QmlGraphicsVisualDataModel::QmlGraphicsVisualDataModel(QmlContext *ctxt)
: QmlGraphicsVisualModel(*(new QmlGraphicsVisualDataModelPrivate(ctxt)))
{
}

QmlGraphicsVisualDataModel::~QmlGraphicsVisualDataModel()
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_listAccessor)
        delete d->m_listAccessor;
    if (d->m_delegateDataType)
        d->m_delegateDataType->release();
}

/*!
    \qmlproperty model VisualDataModel::model
    This property holds the model providing data for the VisualDataModel.

    The model provides a set of data that is used to create the items
    for a view.  For large or dynamic datasets the model is usually
    provided by a C++ model object.  The C++ model object must be a \l
    {QAbstractItemModel} subclass or a simple list.

    Models can also be created directly in QML, using a \l{ListModel} or
    \l{XmlListModel}.

    \sa {qmlmodels}{Data Models}
*/
QVariant QmlGraphicsVisualDataModel::model() const
{
    Q_D(const QmlGraphicsVisualDataModel);
    return d->m_modelVariant;
}

void QmlGraphicsVisualDataModel::setModel(const QVariant &model)
{
    Q_D(QmlGraphicsVisualDataModel);
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
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsInserted(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsInserted(const QModelIndex &,int,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsRemoved(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsRemoved(const QModelIndex &,int,int)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                            this, SLOT(_q_dataChanged(const QModelIndex&,const QModelIndex&)));
        QObject::disconnect(d->m_abstractItemModel, SIGNAL(rowsMoved(const QModelIndex&,int,int,const QModelIndex&,int)),
                            this, SLOT(_q_rowsMoved(const QModelIndex&,int,int,const QModelIndex&,int)));
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

    d->m_roles.clear();
    d->m_roleNames.clear();
    if (d->m_delegateDataType)
        d->m_delegateDataType->release();
    d->m_metaDataCreated = 0;
    d->m_metaDataCacheable = false;
    d->m_delegateDataType = new VDMDelegateDataType(&QmlGraphicsVisualDataModelData::staticMetaObject, d->m_context?d->m_context->engine():qmlEngine(this));

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
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsInserted(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsInserted(const QModelIndex &,int,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsRemoved(const QModelIndex &,int,int)),
                            this, SLOT(_q_rowsRemoved(const QModelIndex &,int,int)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(dataChanged(const QModelIndex&,const QModelIndex&)),
                            this, SLOT(_q_dataChanged(const QModelIndex&,const QModelIndex&)));
        QObject::connect(d->m_abstractItemModel, SIGNAL(rowsMoved(const QModelIndex&,int,int,const QModelIndex&,int)),
                            this, SLOT(_q_rowsMoved(const QModelIndex&,int,int,const QModelIndex&,int)));
        d->m_metaDataCacheable = true;
        return;
    }
    if ((d->m_visualItemModel = qvariant_cast<QmlGraphicsVisualDataModel *>(model))) {
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
    d->m_listAccessor = new QmlListAccessor;
    d->m_listAccessor->setList(model, d->m_context?d->m_context->engine():qmlEngine(this));
    if (d->m_listAccessor->type() != QmlListAccessor::ListProperty)
        d->m_metaDataCacheable = true;
    if (d->m_delegate && d->modelCount()) {
        emit itemsInserted(0, d->modelCount());
        emit countChanged();
    }
}

/*!
    \qmlproperty component VisualDataModel::delegate

    The delegate provides a template defining each item instantiated by a view.
    The index is exposed as an accessible \c index property.  Properties of the
    model are also available depending upon the type of \l {qmlmodels}{Data Model}.

    Here is an example delegate:
    \snippet doc/src/snippets/declarative/listview/listview.qml 0
*/
QmlComponent *QmlGraphicsVisualDataModel::delegate() const
{
    Q_D(const QmlGraphicsVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->delegate();
    return d->m_delegate;
}

void QmlGraphicsVisualDataModel::setDelegate(QmlComponent *delegate)
{
    Q_D(QmlGraphicsVisualDataModel);
    bool wasValid = d->m_delegate != 0;
    d->m_delegate = delegate;
    if (!wasValid && d->modelCount() && d->m_delegate) {
        emit itemsInserted(0, d->modelCount());
        emit countChanged();
    }
    if (wasValid && !d->m_delegate && d->modelCount()) {
        emit itemsRemoved(0, d->modelCount());
        emit countChanged();
    }
}

/*!
    \qmlproperty QModelIndex VisualDataModel::rootIndex

    QAbstractItemModel provides a heirachical tree of data, whereas
    QML only operates on list data. rootIndex allows the children of
    any node in a QAbstractItemModel to be provided by this model.

    This property only affects models of type QAbstractItemModel.

    \code
    // main.cpp
    Q_DECLARE_METATYPE(QModelIndex)

    class MyModel : public QDirModel
    {
        Q_OBJECT
    public:
        MyModel(QmlContext *ctxt) : QDirModel(), context(ctxt) {
            QHash<int,QByteArray> roles = roleNames();
            roles.insert(FilePathRole, "path");
            setRoleNames(roles);
            context->setContextProperty("myModel", this);
            context->setContextProperty("myRoot", QVariant::fromValue(index(0,0,QModelIndex())));
        }

        Q_INVOKABLE void setRoot(const QString &path) {
            QModelIndex root = index(path);
            context->setContextProperty("myRoot", QVariant::fromValue(root));
        }

        QmlContext *context;
    };

    int main(int argc, char ** argv)
    {
        QApplication app(argc, argv);

        QmlView view;
        view.setSource(QUrl("qrc:view.qml"));

        MyModel model(view.rootContext());

        view.execute();
        view.show();

        return app.exec();
    }

    #include "main.moc"
    \endcode

    \code
    // view.qml
    import Qt 4.6

    ListView {
        width: 200
        height: 200
        model: VisualDataModel {
            model: myModel
            rootIndex: myRoot
            delegate: Component {
                Rectangle {
                    height: 25; width: 100
                    Text { text: path }
                    MouseRegion {
                        anchors.fill: parent;
                        onClicked: myModel.setRoot(path)
                    }
                }
            }
        }
    }
    \endcode

*/
QModelIndex QmlGraphicsVisualDataModel::rootIndex() const
{
    Q_D(const QmlGraphicsVisualDataModel);
    return d->m_root;
}

void QmlGraphicsVisualDataModel::setRootIndex(const QModelIndex &root)
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_root != root) {
        int oldCount = d->modelCount();
        d->m_root = root;
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

QString QmlGraphicsVisualDataModel::part() const
{
    Q_D(const QmlGraphicsVisualDataModel);
    return d->m_part;
}

void QmlGraphicsVisualDataModel::setPart(const QString &part)
{
    Q_D(QmlGraphicsVisualDataModel);
    d->m_part = part;
}

int QmlGraphicsVisualDataModel::count() const
{
    Q_D(const QmlGraphicsVisualDataModel);
    return d->modelCount();
}

QmlGraphicsItem *QmlGraphicsVisualDataModel::item(int index, bool complete)
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, d->m_part.toUtf8(), complete);
    return item(index, QByteArray(), complete);
}

/*
  Returns ReleaseStatus flags.
*/
QmlGraphicsVisualDataModel::ReleaseFlags QmlGraphicsVisualDataModel::release(QmlGraphicsItem *item)
{
    Q_D(QmlGraphicsVisualDataModel);
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
        if (inPackage) {
            emit destroyingPackage(qobject_cast<QmlPackage*>(obj));
        } else {
            item->setVisible(false);
            static_cast<QGraphicsItem*>(item)->setParentItem(0);
        }
        stat |= Destroyed;
        obj->deleteLater();
    } else if (!inPackage) {
        stat |= Referenced;
    }

    return stat;
}

QObject *QmlGraphicsVisualDataModel::parts()
{
    Q_D(QmlGraphicsVisualDataModel);
    if (!d->m_parts) 
        d->m_parts = new QmlGraphicsVisualDataModelParts(this);
    return d->m_parts;
}

QmlGraphicsItem *QmlGraphicsVisualDataModel::item(int index, const QByteArray &viewId, bool complete)
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->item(index, viewId, complete);

    if (d->modelCount() <= 0 || !d->m_delegate)
        return 0;
    QObject *nobj = d->m_cache.getItem(index);
    if (!nobj) {
        QmlContext *ccontext = d->m_context;
        if (!ccontext) ccontext = qmlContext(this);
        QmlContext *ctxt = new QmlContext(ccontext);
        QmlGraphicsVisualDataModelData *data = new QmlGraphicsVisualDataModelData(index, this);
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
    QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(nobj);
    if (!item) {
        QmlPackage *package = qobject_cast<QmlPackage *>(nobj);
        if (package) {
            QObject *o = package->part(QString::fromUtf8(viewId));
            item = qobject_cast<QmlGraphicsItem *>(o);
            if (item)
                d->m_packaged.insertMulti(item, package);
        }
    }
    if (!item) {
        d->m_cache.releaseItem(nobj);
        qmlInfo(d->m_delegate) << QmlGraphicsVisualDataModel::tr("Delegate component must be Item type.");
    }

    return item;
}

void QmlGraphicsVisualDataModel::completeItem()
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_visualItemModel) {
        d->m_visualItemModel->completeItem();
        return;
    }

    d->m_delegate->completeCreate();
}

QString QmlGraphicsVisualDataModel::stringValue(int index, const QString &name)
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->stringValue(index, name);

    if ((!d->m_listModelInterface && !d->m_abstractItemModel) || !d->m_delegate)
        return QString();

    QString val;
    QObject *data = 0;
    bool tempData = false;

    if (QObject *nobj = d->m_cache.item(index))
        data = d->data(nobj);
    if (!data) {
        data = new QmlGraphicsVisualDataModelData(index, this);
        tempData = true;
    }

    QmlDeclarativeData *ddata = QmlDeclarativeData::get(data);
    if (ddata && ddata->propertyCache) {
        QmlPropertyCache::Data *prop = ddata->propertyCache->property(name);
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

QVariant QmlGraphicsVisualDataModel::evaluate(int index, const QString &expression, QObject *objectContext)
{
    Q_D(QmlGraphicsVisualDataModel);
    if (d->m_visualItemModel)
        return d->m_visualItemModel->evaluate(index, expression, objectContext);

    if ((!d->m_listModelInterface && !d->m_abstractItemModel) || !d->m_delegate)
        return QVariant();

    QVariant value;
    QObject *nobj = d->m_cache.item(index);
    if (nobj) {
        QmlGraphicsItem *item = qobject_cast<QmlGraphicsItem *>(nobj);
        if (item) {
            QmlExpression e(qmlContext(item), expression, objectContext);
            e.setTrackChange(false);
            value = e.value();
        }
    } else {
        QmlContext *ccontext = d->m_context;
        if (!ccontext) ccontext = qmlContext(this);
        QmlContext *ctxt = new QmlContext(ccontext);
        QmlGraphicsVisualDataModelData *data = new QmlGraphicsVisualDataModelData(index, this);
        ctxt->addDefaultObject(data);
        QmlExpression e(ctxt, expression, objectContext);
        e.setTrackChange(false);
        value = e.value();
        delete data;
        delete ctxt;
    }

    return value;
}

int QmlGraphicsVisualDataModel::indexOf(QmlGraphicsItem *item, QObject *) const
{
    QVariant val = QmlEngine::contextForObject(item)->contextProperty(QLatin1String("index"));
        return val.toInt();
    return -1;
}

void QmlGraphicsVisualDataModel::_q_itemsChanged(int index, int count,
                                         const QList<int> &roles)
{
    Q_D(QmlGraphicsVisualDataModel);
    // XXX - highly inefficient
    for (int ii = index; ii < index + count; ++ii) {

        if (QObject *item = d->m_cache.item(ii)) {
            QmlGraphicsVisualDataModelData *data = d->data(item);

            for (int roleIdx = 0; roleIdx < roles.count(); ++roleIdx) {
                int role = roles.at(roleIdx);
                int propId = data->propForRole(role);
                if (propId != -1) {
                    if (d->m_listModelInterface) {
                        data->setValue(propId, d->m_listModelInterface->data(ii, QList<int>() << role).value(role));
                    } else if (d->m_abstractItemModel) {
                        QModelIndex index = d->m_abstractItemModel->index(ii, 0, d->m_root);
                        data->setValue(propId, d->m_abstractItemModel->data(index, role));
                    }
                }
            }
        }
    }
}

void QmlGraphicsVisualDataModel::_q_itemsInserted(int index, int count)
{
    Q_D(QmlGraphicsVisualDataModel);
    // XXX - highly inefficient
    QHash<int,QmlGraphicsVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int,QmlGraphicsVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= index) {
            QmlGraphicsVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() + count;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QmlGraphicsVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsInserted(index, count);
    emit countChanged();
}

void QmlGraphicsVisualDataModel::_q_itemsRemoved(int index, int count)
{
    Q_D(QmlGraphicsVisualDataModel);
    // XXX - highly inefficient
    QHash<int, QmlGraphicsVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int, QmlGraphicsVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {
        if (iter.key() >= index && iter.key() < index + count) {
            QmlGraphicsVisualDataModelPrivate::ObjectRef objRef = *iter;
            iter = d->m_cache.erase(iter);
            items.insertMulti(-1, objRef); //XXX perhaps better to maintain separately
            QmlGraphicsVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(-1);
        } else if (iter.key() >= index + count) {
            QmlGraphicsVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - count;
            iter = d->m_cache.erase(iter);
            items.insert(index, objRef);
            QmlGraphicsVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }

    d->m_cache.unite(items);
    emit itemsRemoved(index, count);
    emit countChanged();
}

void QmlGraphicsVisualDataModel::_q_itemsMoved(int from, int to, int count)
{
    Q_D(QmlGraphicsVisualDataModel);
    // XXX - highly inefficient
    QHash<int,QmlGraphicsVisualDataModelPrivate::ObjectRef> items;
    for (QHash<int,QmlGraphicsVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        if (iter.key() >= from && iter.key() < from + count) {
            QmlGraphicsVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() - from + to;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QmlGraphicsVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    for (QHash<int,QmlGraphicsVisualDataModelPrivate::ObjectRef>::Iterator iter = d->m_cache.begin();
        iter != d->m_cache.end(); ) {

        int diff = from > to ? count : -count;
        if (iter.key() >= qMin(from,to) && iter.key() < qMax(from+count,to+count)) {
            QmlGraphicsVisualDataModelPrivate::ObjectRef objRef = *iter;
            int index = iter.key() + diff;
            iter = d->m_cache.erase(iter);

            items.insert(index, objRef);

            QmlGraphicsVisualDataModelData *data = d->data(objRef.obj);
            data->setIndex(index);
        } else {
            ++iter;
        }
    }
    d->m_cache.unite(items);

    emit itemsMoved(from, to, count);
}

void QmlGraphicsVisualDataModel::_q_rowsInserted(const QModelIndex &parent, int begin, int end)
{
    if (!parent.isValid())
        _q_itemsInserted(begin, end - begin + 1);
}

void QmlGraphicsVisualDataModel::_q_rowsRemoved(const QModelIndex &parent, int begin, int end)
{
    if (!parent.isValid())
        _q_itemsRemoved(begin, end - begin + 1);
}

void QmlGraphicsVisualDataModel::_q_rowsMoved(const QModelIndex &sourceParent, int sourceStart, int sourceEnd, const QModelIndex &destinationParent, int destinationRow)
{
    const int count = sourceEnd - sourceStart + 1;
    if (!destinationParent.isValid() && !sourceParent.isValid()) {
        _q_itemsMoved(sourceStart, destinationRow, count);
    } else if (!sourceParent.isValid()) {
        _q_itemsRemoved(sourceStart, count);
    } else if (!destinationParent.isValid()) {
        _q_itemsInserted(destinationRow, count);
    }
}

void QmlGraphicsVisualDataModel::_q_dataChanged(const QModelIndex &begin, const QModelIndex &end)
{
    Q_D(QmlGraphicsVisualDataModel);
    if (!begin.parent().isValid())
        _q_itemsChanged(begin.row(), end.row() - begin.row() + 1, d->m_roles);
}

void QmlGraphicsVisualDataModel::_q_createdPackage(int index, QmlPackage *package)
{
    Q_D(QmlGraphicsVisualDataModel);
    emit createdItem(index, qobject_cast<QmlGraphicsItem*>(package->part(d->m_part)));
}

void QmlGraphicsVisualDataModel::_q_destroyingPackage(QmlPackage *package)
{
    Q_D(QmlGraphicsVisualDataModel);
    emit destroyingItem(qobject_cast<QmlGraphicsItem*>(package->part(d->m_part)));
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QListModelInterface)

#include <qmlgraphicsvisualitemmodel.moc>
