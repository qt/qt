/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include "qdeclarativepropertyvaluesource.h"

#include "qdeclarative.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDeclarativePropertyValueSource
    \brief The QDeclarativePropertyValueSource class is an interface for property value sources such as animations and bindings.

    See \l{Property Value Sources} for information on writing custom property
    value sources.
 */

/*!
    Constructs a QDeclarativePropertyValueSource.
*/
QDeclarativePropertyValueSource::QDeclarativePropertyValueSource()
{
}

/*!
    Destroys the value source.
*/
QDeclarativePropertyValueSource::~QDeclarativePropertyValueSource()
{
}

/*!
    \fn void QDeclarativePropertyValueSource::setTarget(const QDeclarativeProperty &property)
    Set the target \a property for the value source.  This method will
    be called by the QML engine when assigning a value source.
*/

QT_END_NAMESPACE
