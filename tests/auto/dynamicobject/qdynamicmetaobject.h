/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
****************************************************************************/

#ifndef _QDYNAMICMETAOBJECT_H_
#define _QDYNAMICMETAOBJECT_H_

#include <qmetaobject.h>
#include <qobject.h>

class QDynamicMetaObjectData;

class QDynamicMetaObject : public QAbstractDynamicMetaObject
{
public:
    QDynamicMetaObject(const QMetaObject *baseMetaObject);
    QDynamicMetaObject(const QDynamicMetaObject &other);
    virtual ~QDynamicMetaObject();

    QDynamicMetaObject &operator=(const QDynamicMetaObject &that);

    void addSignal(const char *sigName);
    void addSignal(const char *sigName, const QList<QByteArray> &parameterNames);
    void addSlot(const char *slotName);
    void addSlot(const char *slotName, const QList<QByteArray> &parameterNames);
    void addProperty(const char *name, const char *type, const char *notifier);
    void addProperty(const char *name, const char *type, int notifierId=-1);

protected:
    virtual int metaCall(QObject *object, QMetaObject::Call _c, int _id, void **_a);
    virtual void callSlot(QObject *object, int id, const QList<QVariant> &args);
    virtual void writeProperty(QObject *object, int id, const QVariant &value);
    virtual QVariant readProperty(QObject *object, int id) const;

private:
    void detach();
    void updateMetaData();
    QDynamicMetaObject &operator=(const QMetaObject &that);

private:
    QDynamicMetaObjectData *data_ptr;
};

#endif
