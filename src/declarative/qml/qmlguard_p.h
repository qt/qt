/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QMLGUARD_P_H
#define QMLGUARD_P_H

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

QT_BEGIN_NAMESPACE

class QObject;
template<class T>
class QmlGuard
{
    QObject *o;
    QmlGuard<QObject> *next;
    QmlGuard<QObject> **prev;
    friend class QmlDeclarativeData;
public:
    inline QmlGuard();
    inline QmlGuard(T *);
    inline QmlGuard(const QmlGuard<T> &);
    inline virtual ~QmlGuard();

    inline QmlGuard<T> &operator=(const QmlGuard<T> &o);
    inline QmlGuard<T> &operator=(T *);
    
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

private:
    inline void addGuard();
    inline void remGuard();
};

Q_DECLARE_METATYPE(QmlGuard<QObject>);

QT_END_NAMESPACE

#include "qmldeclarativedata_p.h"

QT_BEGIN_NAMESPACE

template<class T>
QmlGuard<T>::QmlGuard()
: o(0), next(0), prev(0)
{
}

template<class T>
QmlGuard<T>::QmlGuard(T *g)
: o(g), next(0), prev(0)
{
    if (o) addGuard();
}

template<class T>
QmlGuard<T>::QmlGuard(const QmlGuard<T> &g)
: o(g.o), next(0), prev(0)
{
    if (o) addGuard();
}

template<class T>
QmlGuard<T>::~QmlGuard()
{
    if (prev) remGuard();
    o = 0;
}

template<class T>
QmlGuard<T> &QmlGuard<T>::operator=(const QmlGuard<T> &g)
{
    if (g.o != o) {
        if (prev) remGuard();
        o = g.o;
        if (o) addGuard();
    }
    return *this;
}

template<class T>
QmlGuard<T> &QmlGuard<T>::operator=(T *g)
{
    if (g != o) {
        if (prev) remGuard();
        o = g;
        if (o) addGuard();
    }
    return *this;
}

QT_END_NAMESPACE

#endif // QMLGUARD_P_H
