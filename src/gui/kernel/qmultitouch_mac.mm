/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6

#include <private/qmultitouch_mac_p.h>
#include <qcursor.h>

QT_BEGIN_NAMESPACE

#ifdef QT_MAC_USE_COCOA

QHash<int, QCocoaTouch*> QCocoaTouch::_currentTouches;
QPointF QCocoaTouch::_screenReferencePos;
QPointF QCocoaTouch::_trackpadReferencePos;
bool QCocoaTouch::_inMouseDraggingState = false;
int QCocoaTouch::_idAssignmentCount = 0;

QCocoaTouch::QCocoaTouch(NSTouch *nstouch)
{
    _identity = int([nstouch identity]);

    if (_currentTouches.size() == 0){
        // A new touch sequence is about to begin:
        _touchPoint.setState(Qt::TouchPointPressed | Qt::TouchPointPrimary);
        NSPoint npos = [nstouch normalizedPosition];
        _trackpadPos = QPointF(npos.x, npos.y);
        // This touch will act as reference for all subsequent
        // touches, so they appear next to each other on screen:
        _trackpadReferencePos = _trackpadPos;
        _screenReferencePos = QCursor::pos();
        _touchPoint.setScreenPos(_screenReferencePos);
        // The first touch should always have 0 as id:
        _touchPoint.setId(0);
        _idAssignmentCount = 0;
        _currentTouches.insert(_identity, this);
    } else {
        // We are already tracking at least one touch point. 
        _touchPoint.setId(++_idAssignmentCount);
        _currentTouches.insert(_identity, this);
        updateTouchData(nstouch, NSTouchPhaseBegan);
    }
}

QCocoaTouch::~QCocoaTouch()
{
    _currentTouches.remove(_identity);
}

void QCocoaTouch::updateTouchData(NSTouch *nstouch, NSTouchPhase phase)
{
    NSPoint npos = [nstouch normalizedPosition];
    _trackpadPos = QPointF(npos.x, npos.y);

    if (_inMouseDraggingState || _currentTouches.size() == 1)
        _touchPoint.setState(toTouchPointState(phase) | Qt::TouchPointPrimary);
    else
        _touchPoint.setState(toTouchPointState(phase));

    // From the normalized position on the trackpad, calculate
    // where on screen the touchpoint should be according to the
    // reference position:
    NSSize dsize = [nstouch deviceSize];
    float ppiX = (_trackpadPos.x() - _trackpadReferencePos.x()) * dsize.width;
    float ppiY = (_trackpadPos.y() - _trackpadReferencePos.y()) * dsize.height;
    QPointF relativePos = _trackpadReferencePos - QPointF(ppiX, 1 - ppiY);
    _touchPoint.setScreenPos(_screenReferencePos - relativePos);
}

QCocoaTouch *QCocoaTouch::findQCocoaTouch(NSTouch *nstouch)
{
    int identity = int([nstouch identity]);
    if (_currentTouches.contains(identity))
        return _currentTouches.value(identity);
    return 0;
}

Qt::TouchPointState QCocoaTouch::toTouchPointState(NSTouchPhase nsState)
{
    Qt::TouchPointState qtState = Qt::TouchPointReleased;
    switch (nsState) {
        case NSTouchPhaseBegan:
            qtState = Qt::TouchPointPressed;
            break;
        case NSTouchPhaseMoved:
            qtState = Qt::TouchPointMoved;
            break;
        case NSTouchPhaseStationary:
            qtState = Qt::TouchPointStationary;
            break;
        case NSTouchPhaseEnded:
        case NSTouchPhaseCancelled:
            qtState = Qt::TouchPointReleased;
            break;
        default:
            break;
    }
    return qtState;
}

void QCocoaTouch::setMouseInDraggingState(bool inDraggingState)
{
    // In mouse dragging state, _all_ fingers control the mouse.
    // As such, all fingers should have the primary flag.
    _inMouseDraggingState = inDraggingState;
}

void QCocoaTouch::validateCurrentTouchList(NSEvent *event)
{
    // If the application shows the task switcher during a touch sequence
    // we get into an inconsistant state. We try to detect that here:
    NSTouchPhase p = NSTouchPhaseAny & ~NSTouchPhaseBegan;
    NSSet *s = [event touchesMatchingPhase:p inView:nil];
    if ([s count] == 0 && !_currentTouches.isEmpty()) {
        // Remove all instances, and basically start from scratch:
        QList<QCocoaTouch *> list = _currentTouches.values();
        foreach (QCocoaTouch *t, _currentTouches.values())
            delete t;
        _currentTouches.clear();
    }
}

QList<QTouchEvent::TouchPoint> QCocoaTouch::getCurrentTouchPointList(NSEvent *event)
{
    validateCurrentTouchList(event);
    // Go through all the touchpoints in cocoa, find, or create, a QCocoaTouch for 
    // them, update them, and return the result:
    QList<QTouchEvent::TouchPoint> touchPoints;

    NSSet *ended = [event touchesMatchingPhase:NSTouchPhaseEnded | NSTouchPhaseCancelled inView:nil];
    for (int i=0; i<int([ended count]); ++i) {
        NSTouch *touch = [[ended allObjects] objectAtIndex:i];
        QCocoaTouch *qcocoaTouch = findQCocoaTouch(touch);
        if (qcocoaTouch) {
            qcocoaTouch->updateTouchData(touch, [touch phase]);
            touchPoints.append(qcocoaTouch->_touchPoint);
            delete qcocoaTouch;
        }
    }

    NSSet *began = [event touchesMatchingPhase:NSTouchPhaseBegan inView:nil];
    for (int i=0; i<int([began count]); ++i) {
        NSTouch *touch = [[began allObjects] objectAtIndex:i];
        QCocoaTouch *qcocoaTouch = findQCocoaTouch(touch);
        if (!qcocoaTouch)
            qcocoaTouch = new QCocoaTouch(touch);
        else
            qcocoaTouch->updateTouchData(touch, [touch phase]);
        touchPoints.append(qcocoaTouch->_touchPoint);
    }

    NSSet *active = [event touchesMatchingPhase:NSTouchPhaseMoved | NSTouchPhaseStationary inView:nil];
    for (int i=0; i<int([active count]); ++i) {
        NSTouch *touch = [[active allObjects] objectAtIndex:i];
        QCocoaTouch *qcocoaTouch = findQCocoaTouch(touch);
        if (qcocoaTouch) {
            qcocoaTouch->updateTouchData(touch, [touch phase]);
            touchPoints.append(qcocoaTouch->_touchPoint);
        }
    }

    return touchPoints;
}

#endif

QT_END_NAMESPACE

#endif // MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6

