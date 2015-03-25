/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativeparserstatus.h"

QT_BEGIN_NAMESPACE

/*!
    \class QDeclarativeParserStatus
    \since 4.7
    \brief The QDeclarativeParserStatus class provides updates on the QML parser state.

    QDeclarativeParserStatus provides a mechanism for classes instantiated by
    a QDeclarativeEngine to receive notification at key points in their creation.

    This class is often used for optimization purposes, as it allows you to defer an
    expensive operation until after all the properties have been set on an
    object. For example, QML's \l {Text} element uses the parser status
    to defer text layout until all of its properties have been set (we
    don't want to layout when the \c text is assigned, and then relayout
    when the \c font is assigned, and relayout again when the \c width is assigned,
    and so on).

    To use QDeclarativeParserStatus, you must inherit both a QObject-derived class
    and QDeclarativeParserStatus, and use the Q_INTERFACES() macro.

    \code
    class MyObject : public QObject, public QDeclarativeParserStatus
    {
        Q_OBJECT
        Q_INTERFACES(QDeclarativeParserStatus)

    public:
        MyObject(QObject *parent = 0);
        ...
        void classBegin();
        void componentComplete();
    }
    \endcode
*/

/*! \internal */
QDeclarativeParserStatus::QDeclarativeParserStatus()
: d(0)
{
}

/*! \internal */
QDeclarativeParserStatus::~QDeclarativeParserStatus()
{
    if(d)
        (*d) = 0;
}

/*!
    \fn void QDeclarativeParserStatus::classBegin()

    Invoked after class creation, but before any properties have been set.
*/

/*!
    \fn void QDeclarativeParserStatus::componentComplete()

    Invoked after the root component that caused this instantiation has 
    completed construction.  At this point all static values and binding values
    have been assigned to the class.
*/

QT_END_NAMESPACE
