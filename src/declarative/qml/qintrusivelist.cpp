/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qintrusivelist_p.h"

/*!
\class QIntrusiveList
\brief The QIntrusiveList class is a template class that provides a list of objects using static storage.
\internal

QIntrusiveList creates a linked list of objects.  Adding and removing objects from the 
QIntrusiveList is a constant time operation and is very quick.  The list performs no memory
allocations, but does require the objects being added to the list to contain a QIntrusiveListNode
instance for the list's use.  Even so, for small lists QIntrusiveList uses less memory than Qt's
other list classes.

As QIntrusiveList uses storage inside the objects in the list, each object can only be in one
list at a time.  Objects are inserted by the insert() method.  If the object is already
in a list (including the one it is being inserted into) it is first removed, and then inserted
at the head of the list.  QIntrusiveList is a last-in-first-out list.  That is, following an
insert() the inserted object becomes the list's first() object.

\code
struct MyObject {
    MyObject(int value) : value(value) {}

    int value;
    QIntrusiveListNode node;
};
typedef QIntrusiveList<MyObject, &MyObject::node> MyObjectList;

void foo() {
    MyObjectList list;

    MyObject m0(0);
    MyObject m1(1);
    MyObject m2(2);

    list.insert(&m0);
    list.insert(&m1);
    list.insert(&m2);

    // QIntrusiveList is LIFO, so will print: 2... 1... 0...
    for (MyObjectList::iterator iter = list.begin(); iter != list.end(); ++iter) {
        qWarning() << iter->value;
    }
}
\endcode
*/


/*!
\fn QIntrusiveList::QIntrusiveList();

Construct an empty list.
*/

/*!
\fn QIntrusiveList::~QIntrusiveList();

Destroy the list.  All entries are removed.
*/

/*!
\fn void QIntrusiveList::insert(N *object);

Insert \a object into the list.  If \a object is a member of this, or another list, it will be 
removed and inserted at the head of this list.
*/

/*!
\fn void QIntrusiveList::remove(N *object);

Remove \a object from the list.  \a object must not be null.
*/

/*!
\fn N *QIntrusiveList::first() const

Returns the first entry in this list, or null if the list is empty.
*/

/*!
\fn N *QIntrusiveList::next(N *current)

Returns the next object after \a current, or null if \a current is the last object.  \a current cannot be null.
*/

/*!
\fn iterator QIntrusiveList::begin()

Returns an STL-style interator pointing to the first item in the list.

\sa end()
*/

/*!
\fn iterator QIntrusiveList::end()

Returns an STL-style iterator pointing to the imaginary item after the last item in the list.

\sa begin()
*/

/*!
iterator &QInplacelist::iterator::erase()

Remove the current object from the list, and return an iterator to the next element.
*/


/*!
\fn QIntrusiveListNode::QIntrusiveListNode()

Create a QIntrusiveListNode.
*/

/*!
\fn QIntrusiveListNode::~QIntrusiveListNode()

Destroy the QIntrusiveListNode.  If the node is in a list, it is removed.
*/

/*!
\fn void QIntrusiveListNode::remove()

If in a list, remove this node otherwise do nothing.
*/

/*!
\fn bool QIntrusiveListNode::isInList() const

Returns true if this node is in a list, false otherwise.
*/

