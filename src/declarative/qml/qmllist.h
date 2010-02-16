/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMLLIST_H
#define QMLLIST_H

#include "qmlprivate.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

template<typename T>
class QmlList : private QmlPrivate::ListInterface
{
public:
    virtual void append(T) = 0;
    virtual void insert(int, T) = 0;
    virtual void removeAt(int) = 0;
    virtual T at(int) const = 0;
    virtual int count() const = 0;
    virtual void clear() = 0;
    QmlList<T> &operator<<(T t) { append(t); return *this; }

protected:
    virtual int type() const  { return qMetaTypeId<T>(); }
    virtual void append(void *d) { const T &v = *(T *)d; append(v); }
    virtual void insert(int i, void *d) { const T &v = *(T *)d; insert(i, v); }
    virtual void at(int i, void *p) const { const T &v = at(i); *((T*)p) = v; }
};

template<typename T>
class QmlConcreteList : public QList<T>, public QmlList<T>
{
public:
    virtual void append(T v) { QList<T>::append(v); }
    virtual void insert(int i, T v) { QList<T>::insert(i, v); }
    virtual void clear() { QList<T>::clear(); }
    virtual T at(int i) const { return QList<T>::at(i); }
    virtual void removeAt(int i) { QList<T>::removeAt(i); }
    virtual int count() const { return QList<T>::count(); }
};

#define QML_DECLARE_LIST_PROXY(ClassName, ListType, ListName) \
class Qml_ProxyList_ ##ListName : public QmlList<ListType> \
{ \
    public: \
        virtual void removeAt(int idx) \
        { \
            ClassName *p = (ClassName *)((char *)this + ((char *)(ClassName *)(0x10000000) - (char *)&((ClassName *)(0x10000000))->ListName)); \
            p->ListName ## _removeAt(idx); \
        } \
        virtual int count() const \
        { \
            ClassName *p = (ClassName *)((char *)this + ((char *)(ClassName *)(0x10000000) - (char *)&((ClassName *)(0x10000000))->ListName)); \
            return p->ListName ## _count(); \
        } \
        virtual void append(ListType v) \
        { \
            ClassName *p = (ClassName *)((char *)this + ((char *)(ClassName *)(0x10000000) - (char *)&((ClassName *)(0x10000000))->ListName)); \
            p->ListName ## _append(v); \
        } \
        virtual void insert(int idx, ListType v) \
        { \
            ClassName *p = (ClassName *)((char *)this + ((char *)(ClassName *)(0x10000000) - (char *)&((ClassName *)(0x10000000))->ListName)); \
            p->ListName ## _insert(idx, v); \
        } \
        virtual ListType at(int idx) const \
        { \
            ClassName *p = (ClassName *)((char *)this + ((char *)(ClassName *)(0x10000000) - (char *)&((ClassName *)(0x10000000))->ListName)); \
            return p->ListName ## _at(idx); \
        } \
        virtual void clear() \
        { \
            ClassName *p = (ClassName *)((char *)this + ((char *)(ClassName *)(0x10000000) - (char *)&((ClassName *)(0x10000000))->ListName)); \
            p->ListName ## _clear(); \
        } \
}; \
friend class Qml_ProxyList_ ##ListName ; \
Qml_ProxyList_##ListName ListName;

template<typename T>
struct QmlListProperty {
    typedef void (*AppendFunction)(QmlListProperty<T> *, T*);
    typedef int (*CountFunction)(QmlListProperty<T> *);
    typedef T *(*AtFunction)(QmlListProperty<T> *, int);
    typedef void (*ClearFunction)(QmlListProperty<T> *);
    typedef void (*InsertFunction)(QmlListProperty<T> *, int, T *);
    typedef void (*RemoveAtFunction)(QmlListProperty<T> *, int);

    QmlListProperty() 
        : version(1), object(0), data(0), append(0), count(0), at(0), clear(0), insert(0), removeAt(0) {}
    QmlListProperty(QObject *o, QList<T *> &list)
        : version(1), object(o), data(&list), append(qlist_append), count(qlist_count), at(qlist_at),
          clear(qlist_clear), insert(qlist_insert), removeAt(qlist_removeAt) {}
    QmlListProperty(QObject *o, void *d, AppendFunction a, CountFunction c = 0, AtFunction t = 0, 
            ClearFunction l = 0, InsertFunction i = 0, RemoveAtFunction r = 0)
        : version(1), object(o), data(d), append(a), count(c), at(t), clear(l), insert(i), removeAt(r) {}

    bool operator==(const QmlListProperty &o) const {
        return version == o.version &&
               object == o.object &&
               data == o.data &&
               append == o.append &&
               count == o.count &&
               at == o.at &&
               clear == o.clear &&
               insert == o.insert &&
               removeAt == o.removeAt;
    }

    int version;

    QObject *object;
    void *data;
    
    AppendFunction append;

    CountFunction count;
    AtFunction at;

    ClearFunction clear;

    InsertFunction insert;
    RemoveAtFunction removeAt;

private:
    static void qlist_append(QmlListProperty *p, T *v) {
        ((QList<T *> *)p->data)->append(v); 
    }
    static int qlist_count(QmlListProperty *p) {
        return ((QList<T *> *)p->data)->count();
    }
    static T *qlist_at(QmlListProperty *p, int idx) {
        return ((QList<T *> *)p->data)->at(idx);
    }
    static void qlist_clear(QmlListProperty *p) {
        return ((QList<T *> *)p->data)->clear();
    }
    static void qlist_insert(QmlListProperty *p, int idx, T *v) {
        return ((QList<T *> *)p->data)->insert(idx, v);
    }
    static void qlist_removeAt(QmlListProperty *p, int idx) {
        return ((QList<T *> *)p->data)->removeAt(idx);
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLLIST_H
