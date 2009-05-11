/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qlistmodelinterface.h"

QT_BEGIN_NAMESPACE

/*!
  \class QListModelInterface
  \brief The QListModelInterface class can be subclassed to provide C++ models to QFx Views

  This class is comprised primarily of pure virtual functions which
  you must implement in a subclass. You can then use the subclass
  as a model for a QFx view, such as a QFxListView.
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

/*! \fn QHash_int QListModelInterface::data(int index, const QList<int> &roles) const
  Returns the data at the given \a index for the specifed \a roles.
*/

/*! \fn bool QListModelInterface::setData(int index, const QHash<int> &values)
  Sets the data at the given \a index. \a values is a mapping of
  QVariant values to roles.
*/

/*! \fn QList<int> QListModelInterface::roles() const
  Returns the list of roles for which the list model interface
  provides data.
*/

/*! \fn QString QListModelInterface::toString(int role) const
  Returns a string description of the specified \a role.
*/

/*! \enum QListModelInterface::Roles

  Values for representing roles.

  \value TextRole

  \value IconRole
*/ 

QT_END_NAMESPACE
