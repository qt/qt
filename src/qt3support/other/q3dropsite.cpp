/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3Support module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "q3dropsite.h"

#ifndef QT_NO_DRAGANDDROP

#include "qwidget.h"

QT_BEGIN_NAMESPACE

/*!
  \class Q3DropSite
  \brief The Q3DropSite class provides nothing and does nothing.

  \compat

  It was used in Qt 1.x to do some drag and drop; that has since been
  folded into QWidget.

  \sa Q3DragObject, Q3TextDrag, Q3ImageDrag
*/

/*!
  Constructs a Q3DropSite to handle events for the widget \a self.

  Pass \c this as the \a self parameter.
  This enables dropping by calling QWidget::setAcceptDrops(true).
*/
Q3DropSite::Q3DropSite(QWidget* self)
{
    self->setAcceptDrops(true);
}

/*!
  Destroys the drop site.
*/
Q3DropSite::~Q3DropSite()
{
}

QT_END_NAMESPACE

#endif // QT_NO_DRAGANDDROP
