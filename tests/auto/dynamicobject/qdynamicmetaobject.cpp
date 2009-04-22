/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
****************************************************************************/

#include <qcoreapplication.h>

#include <qmetaobject.h>

#include "qobject.h"
#include "private/qobject_p.h"
#include "qmetaobjectbuilder.h"
#include "qdynamicmetaobject.h"

class QDynamicMetaObjectData
{
public:
    QDynamicMetaObjectData() : ref(1), baseMeta(0), meta(0) {}
    ~QDynamicMetaObjectData() { qFree(meta); }

    QAtomicInt ref;
    QMetaObjectBuilder builder;
    const QMetaObject *baseMeta;
    QMetaObject *meta;
};

/*!
    \class QDynamicMetaObject
    \brief The QDynamicMetaObject class allows adding signals, slots and properties to
    QObjects at runtime.

    Any QObject derived class can be extended by creating a QDynamicMetaObject
    based on the QObject's QMetaObject.

    QObject::setDynamicMetaObject() is called to set the dynamic meta-object for
    the QObject.
    
    \sa QMetaObject
*/

/*!
    Constructs a dynamic meta-object based on \a baseMetaObject.

    QObject::setDynamicMetaObject() should be called to install it.
*/
QDynamicMetaObject::QDynamicMetaObject(const QMetaObject *baseMetaObject)
{
    data_ptr = new QDynamicMetaObjectData;
    data_ptr->baseMeta = baseMetaObject;
    data_ptr->builder.setSuperClass(data_ptr->baseMeta);
    data_ptr->builder.setClassName(data_ptr->baseMeta->className());
    updateMetaData();
}

/*!
    Constructs a dynamic meta-object that is a copy of \a other.
*/
QDynamicMetaObject::QDynamicMetaObject(const QDynamicMetaObject &other)
{
    data_ptr = other.data_ptr;
    data_ptr->ref.ref();
}

/*!
    Destroys the dynamic meta-ovject.
*/
QDynamicMetaObject::~QDynamicMetaObject()
{
    if (!data_ptr->ref.deref())
        delete data_ptr;
}

/*!
    Copies the dynamic metadata from \a that.  The static metadata must be identical,
    i.e. the dynamic metadata is constructed for the same class.
*/
QDynamicMetaObject &QDynamicMetaObject::operator=(const QDynamicMetaObject &that)
{
    Q_ASSERT(data_ptr->baseMeta == that.data_ptr->baseMeta);
    qAtomicAssign(data_ptr, that.data_ptr);
    return *this;
}

/*!
    \internal
*/
QDynamicMetaObject &QDynamicMetaObject::operator=(const QMetaObject &that)
{
    static_cast<QMetaObject*>(this)->d = that.d;
    return *this;
}

/*!
    \internal
*/
void QDynamicMetaObject::detach()
{
    if (data_ptr->ref == 1)
        return;

    //XXX copy builder faster.
    QByteArray buf;
    QDataStream ds(&buf, QIODevice::ReadWrite);
    data_ptr->builder.serialize(ds);
    if (!data_ptr->ref.deref())
        delete data_ptr;
    data_ptr = new QDynamicMetaObjectData;
    QMap<QByteArray, const QMetaObject *> refs;
    data_ptr->builder.deserialize(ds, refs);
    updateMetaData();
}

/*!
    Adds the signal with \a signature.
*/
void QDynamicMetaObject::addSignal(const char *signature)
{
    detach();
    data_ptr->builder.addSignal(signature);
    updateMetaData();
}

/*!
    Adds the signal with \a signature and sets the \a parameterNames.
*/
void QDynamicMetaObject::addSignal(const char *signature, const QList<QByteArray> &parameterNames)
{
    detach();
    QMetaMethodBuilder method = data_ptr->builder.addSignal(signature);
    method.setParameterNames(parameterNames);
    updateMetaData();
}

/*!
    Adds the slot with \a signature.
*/
void QDynamicMetaObject::addSlot(const char *signature)
{
    detach();
    data_ptr->builder.addSlot(signature);
    updateMetaData();
}


/*!
    Adds the slot with \a signature and sets the \a parameterNames.
*/
void QDynamicMetaObject::addSlot(const char *signature, const QList<QByteArray> &parameterNames)
{
    detach();
    QMetaMethodBuilder method = data_ptr->builder.addSlot(signature);
    method.setParameterNames(parameterNames);
    updateMetaData();
}

/*!
    Adds the property with \a name of \a type.  If \a notifier specified it is
    set as the change notifier for the property.
*/
void QDynamicMetaObject::addProperty(const char *name, const char *type, const char *notifier)
{
    int notifierId = -1;
    if (notifier)
        notifierId = data_ptr->builder.indexOfSignal(notifier);
    data_ptr->builder.addProperty(name, type, notifierId);
    updateMetaData();
}

/*!
    Adds the property with \a name of \a type.  If \a notifierId is specified it is
    set as the change notifier for the property.
*/
void QDynamicMetaObject::addProperty(const char *name, const char *type, int notifierId)
{
    data_ptr->builder.addProperty(name, type, notifierId);
    updateMetaData();
}

/*!
    \reimp
*/
int QDynamicMetaObject::metaCall(QObject *object, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod && _id >= data_ptr->baseMeta->methodCount()) {
        QMetaMethod metaMethod = method(_id);
        if (metaMethod.methodType() == QMetaMethod::Slot) {
            if (metaMethod.parameterTypes().count() == 0) {
                static QList<QVariant> noArgs;
                callSlot(object, _id, noArgs);
            } else {
                QList<QVariant> args;
                for (int i = 0; i < metaMethod.parameterTypes().count(); ++i) {
                    int typeId = QVariant::nameToType(metaMethod.parameterTypes().at(i));
                    args.append(QVariant(typeId, _a[i+1]));
                }
                callSlot(object, _id, args);
            }
        } else if (metaMethod.methodType() == QMetaMethod::Signal)
            activate(object, _id, _a);
        _id = -1;
    } else if (_c == QMetaObject::ReadProperty && _id >= data_ptr->baseMeta->propertyCount()) {
        *(reinterpret_cast<QVariant*>(_a[1])) = readProperty(object, _id);
        _id = -1;
    } else if (_c == QMetaObject::WriteProperty && _id >= data_ptr->baseMeta->propertyCount()) {
        writeProperty(object, _id, *(reinterpret_cast<QVariant*>(_a[1])));
        if (property(_id).hasNotifySignal())
            activate(object, property(_id).notifySignalIndex(), 0);
        _id = -1;
    } else {
        _id = object->qt_metacall(_c, _id, _a);
    }

    return _id;
}

/*!
    The slot with \a id has been called for \a object.  The parameters are available via \a args.
*/
void QDynamicMetaObject::callSlot(QObject *object, int id, const QList<QVariant> &args)
{
    Q_UNUSED(object);
    Q_UNUSED(id);
    Q_UNUSED(args);
}

/*!
    The \a value of property with \a id has changed for \a object.
*/
void QDynamicMetaObject::writeProperty(QObject *object, int id, const QVariant &value)
{
    Q_UNUSED(object);
    Q_UNUSED(id);
    Q_UNUSED(value);
}

/*!
    Return the value of the property with \a id belonging to \a object.
*/
QVariant QDynamicMetaObject::readProperty(QObject *object, int id) const
{
    Q_UNUSED(object);
    Q_UNUSED(id);
    return QVariant();
}

void QDynamicMetaObject::updateMetaData()
{
    qFree(data_ptr->meta);
    data_ptr->meta = data_ptr->builder.toMetaObject();
    *this = *data_ptr->meta;
}

