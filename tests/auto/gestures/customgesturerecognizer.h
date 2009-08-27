/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
** contact the sales department at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef CUSTOMGESTURERECOGNIZER_H
#define CUSTOMGESTURERECOGNIZER_H

#include "qgesturerecognizer.h"
#include "qgesture.h"
#include "qevent.h"

class SingleshotEvent : public QEvent
{
public:
    static const int Type = QEvent::User + 1;

    QPoint point;

    explicit SingleshotEvent(int x = 0, int y = 0)
        : QEvent(QEvent::Type(Type)), point(x, y) { }
};

class SingleshotGesture : public QGesture
{
    Q_OBJECT
public:
    SingleshotGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class SingleshotGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    SingleshotGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    SingleshotGesture *gesture;
};

struct TouchPoint {
    enum State
    {
        None = 0,
        Begin = Qt::TouchPointPressed,
        Update = Qt::TouchPointMoved,
        End = Qt::TouchPointReleased
    };
    int id;
    QPoint pt;
    State state;
    TouchPoint() : id(0), state(None) { }
    TouchPoint(int id_, int x_, int y_, State state_) : id(id_), pt(x_, y_), state(state_) { }
};

class TouchEvent : public QEvent
{
public:
    static const int Type = QEvent::User + 2;

    TouchEvent()
        : QEvent(QEvent::Type(Type))
    {
    }

    TouchPoint points[2];
};

class PinchGesture : public QGesture
{
    Q_OBJECT
public:
    PinchGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    TouchPoint startPoints[2];
    TouchPoint lastPoints[2];
    TouchPoint points[2];

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class PinchGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    PinchGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    PinchGesture *gesture;
};

class SecondFingerGesture : public QGesture
{
    Q_OBJECT
public:
    SecondFingerGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    TouchPoint startPoint;
    TouchPoint lastPoint;
    TouchPoint point;

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class SecondFingerGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    SecondFingerGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    SecondFingerGesture *gesture;
};

class PanGesture : public QGesture
{
    Q_OBJECT
public:
    PanGesture(QObject *parent, const QString &type)
        : QGesture(parent, type) { }

    TouchPoint startPoints[2];
    TouchPoint lastPoints[2];
    TouchPoint points[2];

    QPoint offset;

protected:
    void translate(const QPoint &pt)
    {
        offset += pt;
    }
};

class PanGestureRecognizer : public QGestureRecognizer
{
    Q_OBJECT
public:
    static const char *Name;

    PanGestureRecognizer(QObject *parent = 0);

    QGestureRecognizer::Result filterEvent(const QEvent *event);
    QGesture* getGesture() { return gesture; }
    void reset();

private:
    PanGesture *gesture;
};

#endif
