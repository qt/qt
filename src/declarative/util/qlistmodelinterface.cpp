/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclaractive module of the Qt Toolkit.
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

#include "private/qlistmodelinterface_p.h"

QT_BEGIN_NAMESPACE

/*!
  \internal
  \class QListModelInterface
  \since 4.7
  \brief The QListModelInterface class can be subclassed to provide C++ models to QDeclarativeGraphics Views

  This class is comprised primarily of pure virtual functions which
  you must implement in a subclass. You can then use the subclass
  as a model for a QDeclarativeGraphics view, such as a QDeclarativeListView.
*/

/*! \fn QListModelInterface::QListModelInterface(QObject *parent)
  Constructs a QListModelInterface with the specified \a parent.
*/

  /*! \fn QListModelInterface::QListModelInterface(QObjectPrivate &dd, QObject *parent)

  \internal
 */

/*! \fn QListModelInterface::~QListModelInterface()
  The destructor is virtual.
 */

/*! \fn int QListModelInterface::count() const
  Returns the number of data entries in the model.
*/

/*! \fn QVariant QListModelInterface::data(int index, int role) const
  Returns the data at the given \a index for the specified \a roles.
*/

/*! \fn QList<int> QListModelInterface::roles() const
  Returns the list of roles for which the list model interface
  provides data.
*/

/*! \fn QString QListModelInterface::toString(int role) const
  Returns a string description of the specified \a role.
*/

/*! \fn void QListModelInterface::itemsInserted(int index, int count)
  Emit this signal when \a count items are inserted at \a index.
 */

/*! \fn void QListModelInterface::itemsRemoved(int index, int count)
  Emit this signal when \a count items are removed at \a index.
 */

/*! \fn void QListModelInterface::itemsMoved(int from, int to, int count)
  Emit this signal when \a count items are moved from index \a from
  to index \a to.
 */

/*! \fn void QListModelInterface::itemsChanged(int index, int count, const QList<int> &roles)
  Emit this signal when \a count items at \a index have had their
  \a roles changed.
 */

QT_END_NAMESPACE
