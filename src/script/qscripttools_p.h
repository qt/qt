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


#ifndef QSCRIPTTOOLS_P_H
#define QSCRIPTTOOLS_P_H

template<class T>
class QScriptBagContainer;

/*!
  \internal
  \interface
  Helper class for a container. The purpuse of it is to add two pointer properties to a class
  inheriting this class without bloating an interface.

  This class exist only as a memory storage implementation. The only way to use it is to inherit it.
*/
class QScriptLinkedNode
{
protected:
    QScriptLinkedNode()
        : m_next(0)
        , m_prev(0)
    {}
private:
    template<class T>
    friend class QScriptBagContainer;
    QScriptLinkedNode *m_next;
    QScriptLinkedNode *m_prev;
};

/*!
  \internal
  The QScriptBagContainer is a simple, low level, set like container for a pointer type castable to
  QScriptLinkedNode*.
  Algorithms complexity:
  put: O(1)
  get: O(1)
  forEach: O(n)
  \note This container doesn't take ownership of pointed values.
*/
template<class T>
class QScriptBagContainer
{
public:
    QScriptBagContainer()
        : m_first(0)
        , m_last(0)
    {}

    /*!
      \internal
      Add a this \a value to this container
    */
    void insert(T* value)
    {
        QScriptLinkedNode* v = static_cast<QScriptLinkedNode*>(value);
        Q_ASSERT(v);
        if (!m_last) {
            Q_ASSERT(!m_first);
            m_last = v;
        } else
            m_first->m_prev = v;

        v->m_next = m_first;
        v->m_prev = 0;
        m_first = v;
    }

    /*!
      \internal
      Remove this \a value from this container
    */
    void remove(T* value)
    {
        QScriptLinkedNode* v = static_cast<QScriptLinkedNode*>(value);
        Q_ASSERT(v);
        if (v->m_next)
            v->m_next->m_prev= v->m_prev;
        else
            m_last = v->m_prev;

        if (v->m_prev)
            v->m_prev->m_next = v->m_next;
        else
            m_first = v->m_next;
    }

    /*!
      \internal
      Call \a fun for each element in this container. Fun should accept T* as a parameter.
      \note It is not allowed to change this container (by calling put or get functions).
    */
    template<class Functor>
    void forEach(Functor fun)
    {
        QScriptLinkedNode *i = m_first;
        QScriptLinkedNode *tmp;
        while (i) {
            tmp = i;
            i = i->m_next;
            fun(static_cast<T*>(tmp));
        }
    }

    /*!
      \internal
      Clear this container.
    */
    void clear()
    {
        m_first = m_last = 0;
    }

//    void dump(const char* msg) const
//    {
//        qDebug() << msg;
//        qDebug() << m_first << " => " << m_last;
//        QScriptLinkedNode *i = m_first;
//        while (i) {
//            qDebug() <<"  - " << i << "(" << i->m_prev << ", " << i->m_next <<")";
//            i = i->m_next;
//        }
//    }

private:
    QScriptLinkedNode *m_first;
    QScriptLinkedNode *m_last;
};

#endif //QSCRIPTTOOLS_P_H
