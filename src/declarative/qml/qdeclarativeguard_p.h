/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEGUARD_P_H
#define QDECLARATIVEGUARD_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of qapplication_*.cpp, qwidget*.cpp and qfiledialog.cpp.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qglobal.h>
#include <QtCore/qvariant.h>
#include <private/qdeclarativedata_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGuardImpl 
{
public:
    inline QDeclarativeGuardImpl();
    inline QDeclarativeGuardImpl(QObject *);
    inline QDeclarativeGuardImpl(const QDeclarativeGuardImpl &);
    inline ~QDeclarativeGuardImpl();

    QObject *o;
    QDeclarativeGuardImpl  *next;
    QDeclarativeGuardImpl **prev;

    inline void addGuard();
    inline void remGuard();
};

class QObject;
template<class T>
class QDeclarativeGuard : private QDeclarativeGuardImpl
{
    friend class QDeclarativeData;
public:
    inline QDeclarativeGuard();
    inline QDeclarativeGuard(T *);
    inline QDeclarativeGuard(const QDeclarativeGuard<T> &);
    inline virtual ~QDeclarativeGuard();

    inline QDeclarativeGuard<T> &operator=(const QDeclarativeGuard<T> &o);
    inline QDeclarativeGuard<T> &operator=(T *);

    inline T *object() const;
    inline void setObject(T *g);
    
    inline bool isNull() const
        { return !o; }

    inline T* operator->() const
        { return static_cast<T*>(const_cast<QObject*>(o)); }
    inline T& operator*() const
        { return *static_cast<T*>(const_cast<QObject*>(o)); }
    inline operator T*() const
        { return static_cast<T*>(const_cast<QObject*>(o)); }
    inline T* data() const
        { return static_cast<T*>(const_cast<QObject*>(o)); }

protected:
    virtual void objectDestroyed(T *) {}
};

QDeclarativeGuardImpl::QDeclarativeGuardImpl()
: o(0), next(0), prev(0)
{
}

QDeclarativeGuardImpl::QDeclarativeGuardImpl(QObject *g)
: o(g), next(0), prev(0)
{
    if (o) addGuard();
}

QDeclarativeGuardImpl::QDeclarativeGuardImpl(const QDeclarativeGuardImpl &g)
: o(g.o), next(0), prev(0)
{
    if (o) addGuard();
}

QDeclarativeGuardImpl::~QDeclarativeGuardImpl()
{
    if (prev) remGuard();
    o = 0;
}

void QDeclarativeGuardImpl::addGuard()
{
    Q_ASSERT(!prev);

    if (QObjectPrivate::get(o)->wasDeleted) 
        return;

    QDeclarativeData *data = QDeclarativeData::get(o, true);
    next = data->guards;
    if (next) next->prev = &next;
    data->guards = this;
    prev = &data->guards;
}

void QDeclarativeGuardImpl::remGuard()
{
    Q_ASSERT(prev);

    if (next) next->prev = prev;
    *prev = next;
    next = 0;
    prev = 0;
}

template<class T>
QDeclarativeGuard<T>::QDeclarativeGuard()
{
}

template<class T>
QDeclarativeGuard<T>::QDeclarativeGuard(T *g)
: QDeclarativeGuardImpl(g)
{
}

template<class T>
QDeclarativeGuard<T>::QDeclarativeGuard(const QDeclarativeGuard<T> &g)
: QDeclarativeGuardImpl(g)
{
}

template<class T>
QDeclarativeGuard<T>::~QDeclarativeGuard()
{
}

template<class T>
QDeclarativeGuard<T> &QDeclarativeGuard<T>::operator=(const QDeclarativeGuard<T> &g)
{
    setObject(g.object());
    return *this;
}

template<class T>
QDeclarativeGuard<T> &QDeclarativeGuard<T>::operator=(T *g)
{
    setObject(g);
    return *this;
}

template<class T>
T *QDeclarativeGuard<T>::object() const 
{ 
    return static_cast<T *>(o); 
};

template<class T>
void QDeclarativeGuard<T>::setObject(T *g) 
{
    if (g != o) {
        if (prev) remGuard();
        o = g;
        if (o) addGuard();
    }
}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeGuard<QObject>)

#endif // QDECLARATIVEGUARD_P_H
