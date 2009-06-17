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

/*!
    \class QScopedPointer
    \brief The QScopedPointer class stores a pointer to a dynamically allocated object, and deletes it upon destruction.
    \since 4.6
    \reentrant
    \ingroup misc

    Managing heap allocated objects manually is hard and error prone, with the
    common result that code leaks memory and is hard to maintain.
    QScopedPointer is a small utility class that heavily simplifies this by
    assigning stack-based memory ownership to heap allocations, more generally
    called resource acquisition is initialization(RAII).

    QScopedPointer guarantees that the object pointed to will get deleted when
    the current scope dissapears, and it also has no way of releasing
    ownership, hence clearly communicating the lifetime and ownership of the
    object. These guarantees are convenient when reading the code.

    Consider this function which does heap allocations, and have various exit points:

    \snippet doc/src/snippets/code/src_corelib_tools_qscopedpointer.cpp 0

    It's encumbered by the manual delete calls. With QScopedPointer, the code
    can be simplified to:

    \snippet doc/src/snippets/code/src_corelib_tools_qscopedpointer.cpp 1

    The code the compiler generates for QScopedPointer is the same as when
    writing it manually. Code that makes use of \a delete are candidates for
    QScopedPointer usage(and if not, possibly another type of smart pointer
    such as QSharedPointer). QScopedPointer intentionally has no copy
    constructor or assignment operator, such that ownership and lifetime is
    clearly communicated.

    The const qualification on a regular C++ pointer can also be expressed with
    a QScopedPointer:

    \snippet doc/src/snippets/code/src_corelib_tools_qscopedpointer.cpp 2

    \note QScopedPointer does not work with arrays.

    \section1 Forward Declared Pointers

    Classes that are forward declared can be used within QScopedPointer, as
    long as the destructor of the forward declared class is available whenever
    a QScopedPointer needs to clean up.

    Concretely, this means that all classes containing a QScopedPointer that
    points to a forward declared class must have non-inline constructors,
    destructors and assignment operators:

    \snippet doc/src/snippets/code/src_corelib_tools_qscopedpointer.cpp 4

    Otherwise, the compiler output a warning about not being able to destruct
    \c MyPrivateClass.

    \sa QSharedPointer
*/

/*!
    \fn QScopedPointer::QScopedPointer(T *p = 0)

    Constructs this QScopedPointer instance and sets its pointer to \a p.
*/

/*!
    \fn QScopedPointer::~QScopedPointer()

    Destroys this QScopedPointer object. Delete the object its pointer points
    to.
*/

/*!
    \fn T *QScopedPointer::data() const

    Returns the value of the pointer referenced by this object. QScopedPointer
    still owns the object pointed to.
*/

/*!
    \fn T &QScopedPointer::operator*() const

    Provides access to the scoped pointer's object.

    If the contained pointer is \c null, behavior is undefined.
    \sa isNull()
*/

/*!
    \fn T *QScopedPointer::operator->() const

    Provides access to the scoped pointer's object.

    If the contained pointer is \c null, behavior is undefined.

    \sa isNull()
*/

/*!
    \fn QScopedPointer::operator bool() const

    Returns \c true if this object is not \c null. This function is suitable
    for use in \tt if-constructs, like:

    \snippet doc/src/snippets/code/src_corelib_tools_qscopedpointer.cpp 3

    \sa isNull()
*/

/*!
    \fn bool QScopedPointer::operator==(const QScopedPointer<T> &other) const

    Equality operator. Returns true if the scoped pointer \a other
    is pointing to the same object as this pointer, otherwise returns false.
*/


/*!
    \fn bool QScopedPointer::operator!=(const QScopedPointer<T> &other) const

    Inequality operator. Returns true if the scoped pointer \a other
    is not pointing to the same object as this pointer, otherwise returns false.
*/

/*!
    \fn bool QScopedPointer::isNull() const

    Returns \c true if this object is holding a pointer that is \c null.
*/

/*!
    \fn void QScopedPointer::reset(T *other = 0)

    Deletes the existing object it is pointing to if any, and sets its pointer to
    \a other. QScopedPointer now owns \a other and will delete it in its
    destructor.

    If \a other is equal to the value returned by data(), behavior is
    undefined.
*/

/*!
    \fn T *QScopedPointer::take()

    Returns the value of the pointer referenced by this object. The pointer of this
    QScopedPointer object will be reset to \c null.
*/

QT_END_NAMESPACE

#endif
