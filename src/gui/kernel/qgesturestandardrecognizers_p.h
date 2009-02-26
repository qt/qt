/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the $MODULE$ of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
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

#ifndef QGESTURESTANDARDRECOGNIZERS_P_H
#define QGESTURESTANDARDRECOGNIZERS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qevent.h"
#include "qbasictimer.h"
#include "qdebug.h"

#include "qgesturerecognizer.h"
#include "private/qdirectionrecognizer_p.h"

QT_BEGIN_NAMESPACE

/*
class QGestureRecognizerMouseTwoButtons : public QGestureRecognizer
{
    Q_OBJECT
public:
    QGestureRecognizerMouseTwoButtons();
    QGestureRecognizer::Result recognize(const QList<QEvent*> &inputEvents);

    QGesture* makeEvent() const;
    void reset();

private:
    void clear();

    // find the last two button click events
    QMouseEvent* userEvents[4];
};
*/

class QGestureRecognizerPan : public QGestureRecognizer
{
    Q_OBJECT
public:
    QGestureRecognizerPan();

    QGestureRecognizer::Result recognize(const QList<QEvent*> &inputEvents);
    QGesture* makeEvent() const;

    void reset();

private:
    void internalReset();

    QPoint pressedPos;
    QPoint lastPos;
    QPoint currentPos;
    bool mousePressed;
    bool gestureFinished;
    Direction::DirectionType lastDirection;
    Direction::DirectionType currentDirection;
    QDirectionDiagonalRecognizer diagonalRecognizer;
    QDirectionSimpleRecognizer simpleRecognizer;
};

class QDoubleTapGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    QDoubleTapGestureRecognizer();

    QGestureRecognizer::Result recognize(const QList<QEvent*> &inputEvents);
    QGesture* makeEvent() const;
    void reset();

private:
    QPoint pressedPosition;
};

class QLongTapGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    QLongTapGestureRecognizer();

    QGestureRecognizer::Result recognize(const QList<QEvent*> &inputEvents);
    QGesture* makeEvent() const;
    void reset();

protected:
    void timerEvent(QTimerEvent *event);

private:
    QPoint pressedPosition;
    QBasicTimer timer;
    int iteration;
    static const int iterationCount;
    static const int iterationTimeout;
};

/*
class QMultiTouchGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    QMultiTouchGestureRecognizer();

    QMap<Qt::GestureType, int> maybeGestureCompletion();
    QGestureRecognizer::Result recognize(const QList<QEvent*> &inputEvents);
    QGesture* makeEvent() const;
    void reset();

private:
    struct Tap {
        //### should I use QPointF everywhere internally ??
        QPointF startPosition;
        QPointF lastPosition;
        QPointF currentPosition;
    };
    typedef QMap<int, Tap> TapMap;
    TapMap touches;
};
*/

QT_END_NAMESPACE

#endif // QGESTURESTANDARDRECOGNIZERS_P_H
