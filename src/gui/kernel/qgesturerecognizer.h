/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QGESTURERECOGNIZER_H
#define QGESTURERECOGNIZER_H

#include <QtCore/qglobal.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QObject;
class QEvent;
class QGesture;
class Q_GUI_EXPORT QGestureRecognizer
{
public:
    enum ResultFlags
    {
        Ignore            = 0x0001,
        NotGesture        = 0x0002,
        MaybeGesture      = 0x0004,
        GestureTriggered  = 0x0008, // Gesture started or updated
        GestureFinished   = 0x0010,

        ResultState_Mask  = 0x00ff,

        ConsumeEventHint        = 0x0100,
        // StoreEventHint          = 0x0200,
        // ReplayStoredEventsHint  = 0x0400,
        // DiscardStoredEventsHint = 0x0800,

        ResultHint_Mask   = 0xff00
    };
    Q_DECLARE_FLAGS(Result, ResultFlags)

    QGestureRecognizer();
    virtual ~QGestureRecognizer();

    virtual QGesture *createGesture(QObject *target);
    virtual QGestureRecognizer::Result filterEvent(QGesture *state, QObject *watched, QEvent *event) = 0;

    virtual void reset(QGesture *state);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QGestureRecognizer::Result)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGESTURERECOGNIZER_H
