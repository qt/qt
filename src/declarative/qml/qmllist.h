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

#ifndef QMLLIST_H
#define QMLLIST_H

#include "qmlprivate.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

template<typename T>
struct QmlListProperty {
    typedef void (*AppendFunction)(QmlListProperty<T> *, T*);
    typedef int (*CountFunction)(QmlListProperty<T> *);
    typedef T *(*AtFunction)(QmlListProperty<T> *, int);
    typedef void (*ClearFunction)(QmlListProperty<T> *);

    QmlListProperty() 
        : object(0), data(0), append(0), count(0), at(0), clear(0), dummy1(0), dummy2(0) {}
    QmlListProperty(QObject *o, QList<T *> &list)
        : object(o), data(&list), append(qlist_append), count(qlist_count), at(qlist_at),
          clear(qlist_clear), dummy1(0), dummy2(0) {}
    QmlListProperty(QObject *o, void *d, AppendFunction a, CountFunction c = 0, AtFunction t = 0, 
                    ClearFunction r = 0)
        : object(o), data(d), append(a), count(c), at(t), clear(r), dummy1(0), dummy2(0) {}

    bool operator==(const QmlListProperty &o) const {
        return object == o.object &&
               data == o.data &&
               append == o.append &&
               count == o.count &&
               at == o.at &&
               clear == o.clear;
    }

    QObject *object;
    void *data;
    
    AppendFunction append;

    CountFunction count;
    AtFunction at;

    ClearFunction clear;

    void *dummy1;
    void *dummy2;

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
};

class QmlEngine;
class QmlListReferencePrivate;
class Q_DECLARATIVE_EXPORT QmlListReference
{
public:
    QmlListReference();
    QmlListReference(QObject *, const char *property, QmlEngine * = 0);
    QmlListReference(const QmlListReference &);
    QmlListReference &operator=(const QmlListReference &);
    ~QmlListReference();

    bool isValid() const;

    QObject *object() const;
    const QMetaObject *listElementType() const;

    bool canAppend() const;
    bool canAt() const;
    bool canClear() const;
    bool canCount() const;

    bool append(QObject *) const;
    QObject *at(int) const;
    bool clear() const;
    int count() const;

private:
    friend class QmlListReferencePrivate;
    QmlListReferencePrivate* d;
};
Q_DECLARE_METATYPE(QmlListReference);

QT_END_NAMESPACE

QT_END_HEADER

#endif // QMLLIST_H
