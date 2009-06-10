/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QSCOPEDPOINTER_H
#define QSCOPEDPOINTER_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE
QT_MODULE(Core)

template <typename T>
class QScopedPointer
{
public:
    explicit inline QScopedPointer(T *p = 0) : d(p)
    {
    }

    inline ~QScopedPointer()
    {
        delete d;
    }

    inline T &operator*() const
    {
        return *d;
    }

    inline T *operator->() const
    {
        return d;
    }

    inline bool operator==(const QScopedPointer<T> &other) const
    {
        return d == other.d;
    }

    inline bool operator!=(const QScopedPointer<T> &other) const
    {
        return d != other.d;
    }

    inline operator bool() const
    {
        return d;
    }

    inline T *data() const
    {
        return d;
    }

    inline bool isNull() const
    {
        return !d;
    }

    inline void reset(T *other = 0)
    {
        T *oldD = d;
        d = other;
        delete oldD;
    }

    inline T *take()
    {
        T *oldD = d;
        d = 0;
        return oldD;
    }

protected:
    T *d;

private:
    Q_DISABLE_COPY(QScopedPointer)
};

/* internal class - allows special handling for resetting and cleaning the pointer */
template <typename T, typename CustomHandler>
class QScopedCustomPointer : public QScopedPointer<T>
{
public:
    inline QScopedCustomPointer(T *p = 0)
        : QScopedPointer<T>(p)
    {
    }

    inline ~QScopedCustomPointer()
    {
        T *oldD = this->d;
        this->d = 0;
        CustomHandler::cleanup(oldD);
    }

    inline void reset(T *other = 0)
    {
        CustomHandler::reset(this->d, other);
    }

    inline T *&data_ptr()
    {
        return this->d;
    }
};

/* Internal helper class - a handler for QShared* classes, to be used in QScopedCustomPointer */
template <typename T>
class QScopedSharedPointerHandler
{
public:
    static inline void cleanup(T *d)
    {
        if (d && !d->ref.deref())
            delete d;
    }

    static inline void reset(T *&d, T *other)
    {
        T *oldD = d;
        d = other;
        cleanup(oldD);
    }
};

/* Internal. This should really have been a typedef, but you can't have a templated typedef :)
   This class is basically a scoped pointer pointing to a ref-counted object
 */
template <typename T>
class QScopedSharedPointer : public QScopedCustomPointer<T, QScopedSharedPointerHandler<T> >
{
public:
    inline QScopedSharedPointer(T *p = 0)
        : QScopedCustomPointer<T, QScopedSharedPointerHandler<T> >(p)
    {
    }

    inline void detach()
    {
        qAtomicDetach(this->d);
    }

    inline void assign(T *other)
    {
        if (this->d == other)
            return;
        if (other)
            other->ref.ref();
        T *oldD = this->d;
        this->d = other;
        QScopedSharedPointerHandler<T>::cleanup(oldD);
    }
};

QT_END_NAMESPACE
QT_END_HEADER

#endif // QSCOPEDPOINTER_H
