/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QSCRIPTSHAREDDATA_P_H
#define QSCRIPTSHAREDDATA_P_H

#include "qglobal.h"
#include "qshareddata.h"

QT_BEGIN_NAMESPACE

/*!
  \internal
  This class should have the same interface as the QSharedData, but implementation doesn't
  need to be thread safe, so atomic ref count was replaced by normal integer value.
*/
class QScriptSharedData
{
public:
    class ReferenceCounter {
        // FIXME shouldn't it be uint or something longer?
        mutable int m_ref;
        ReferenceCounter(int ref) : m_ref(ref) {}
        ~ReferenceCounter() { Q_ASSERT_X(!m_ref, Q_FUNC_INFO, "Memory problem found"); }
    public:
        bool ref() { return ++m_ref; }
        bool deref() { return --m_ref; }
        friend class QScriptSharedData;
    };

    ReferenceCounter ref;
    inline QScriptSharedData() : ref(0) { }

private:
    Q_DISABLE_COPY(QScriptSharedData)
};


template <class T> class QScriptPassPointer;

// FIXME: that could be reimplemented to not check for a null value.
template<class T>
class QScriptSharedDataPointer : public QExplicitlySharedDataPointer<T>
{
public:
    inline QScriptSharedDataPointer() {}
    explicit QScriptSharedDataPointer(QScriptPassPointer<T> data) : QExplicitlySharedDataPointer<T>(data.give()) {}
    explicit QScriptSharedDataPointer(T *data) : QExplicitlySharedDataPointer<T>(data) {}

    inline QScriptSharedDataPointer<T> &operator=(const QScriptPassPointer<T> &other)
    {
        this->QExplicitlySharedDataPointer<T>::operator =(other.give());
        return *this;
    }
    inline QScriptSharedDataPointer<T> &operator=(T *other)
    {
        this->QExplicitlySharedDataPointer<T>::operator =(other);
        return *this;
    }
};

// FIXME: that could be reimplemented to not check for a null value.
template <class T>
class QScriptPassPointer {
public:
    QScriptPassPointer(T *data) : m_ptr(data) {}
    inline QScriptPassPointer() { m_ptr = 0; }
    inline QScriptPassPointer(const QScriptPassPointer<T> &other) : m_ptr(other.give()) {}
    inline ~QScriptPassPointer() { Q_ASSERT_X(!m_ptr, Q_FUNC_INFO, "Ownership of the QScriptPassPointer hasn't been taken"); }

    inline T &operator*() const { return *m_ptr; }
    inline T *operator->() { return m_ptr; }
    inline T *operator->() const { return m_ptr; }
    inline T *data() const { return m_ptr; }
    inline const T *constData() const { return m_ptr; }

    inline bool operator==(const QScriptPassPointer<T> &other) const { return m_ptr == other.m_ptr; }
    inline bool operator!=(const QScriptPassPointer<T> &other) const { return m_ptr != other.m_ptr; }
    inline bool operator==(const QScriptSharedDataPointer<T> &other) const { return m_ptr == other.m_ptr; }
    inline bool operator!=(const QScriptSharedDataPointer<T> &other) const { return m_ptr != other.m_ptr; }
    inline bool operator==(const T *ptr) const { return m_ptr == ptr; }
    inline bool operator!=(const T *ptr) const { return m_ptr != ptr; }

    inline operator bool () const { return m_ptr != 0; }
    inline bool operator!() const { return !m_ptr; }

    inline QScriptPassPointer<T> & operator=(const QScriptPassPointer<T> &other)
    {
        if (other.m_ptr != m_ptr) {
            if (m_ptr)
                delete m_ptr;
            m_ptr = other.give();
        }
        return *this;
    }

    inline QScriptPassPointer &operator=(T *other)
    {
        if (other != m_ptr) {
            if (m_ptr)
                delete m_ptr;
            m_ptr = other;
        }
        return *this;
    }

    inline T* give() const
    {
        T* result = m_ptr;
        m_ptr = 0;
        return result;
    }

private:
    mutable T* m_ptr;
};

QT_END_NAMESPACE

#endif // QSCRIPTSHAREDDATA_P_H
