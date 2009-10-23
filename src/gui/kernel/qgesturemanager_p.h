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

#ifndef QGESTUREMANAGER_P_H
#define QGESTUREMANAGER_P_H

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

#include "qobject.h"
#include "qbasictimer.h"
#include "private/qwidget_p.h"
#include "qgesturerecognizer.h"

QT_BEGIN_NAMESPACE

class QBasicTimer;
class QGraphicsObject;
class QGestureManager : public QObject
{
    Q_OBJECT
public:
    QGestureManager(QObject *parent);
    ~QGestureManager();

    Qt::GestureType registerGestureRecognizer(QGestureRecognizer *recognizer);
    void unregisterGestureRecognizer(Qt::GestureType type);

    bool filterEvent(QWidget *receiver, QEvent *event);
    bool filterEvent(QGesture *receiver, QEvent *event);
    bool filterEvent(QGraphicsObject *receiver, QEvent *event);

    // declared in qapplication.cpp
    static QGestureManager* instance();

protected:
    void timerEvent(QTimerEvent *event);
    bool filterEventThroughContexts(const QMap<QObject *, Qt::GestureType> &contexts,
                                    QEvent *event);

private:
    QMultiMap<Qt::GestureType, QGestureRecognizer *> recognizers;

    QSet<QGesture *> activeGestures;
    QMap<QGesture *, QBasicTimer> maybeGestures;

    enum State {
        Gesture,
        NotGesture,
        MaybeGesture // this means timers are up and waiting for some
                     // more events, and input events are handled by
                     // gesture recognizer explicitely
    } state;

    struct ObjectGesture
    {
        QWeakPointer<QObject> object;
        Qt::GestureType gesture;

        ObjectGesture(QObject *o, const Qt::GestureType &g) : object(o), gesture(g) { }
        inline bool operator<(const ObjectGesture& rhs) const
        {
            if (object.data() < rhs.object.data())
                return true;
            if (object == rhs.object)
                return gesture < rhs.gesture;
            return false;
        }
    };

    QMap<ObjectGesture, QGesture *> objectGestures;
    QMap<QGesture *, QGestureRecognizer *> gestureToRecognizer;
    QHash<QGesture *, QObject *> gestureOwners;

    QHash<QGesture *, QWidget *> gestureTargets;

    int lastCustomGestureId;

    QGesture *getState(QObject *widget, Qt::GestureType gesture);
    void deliverEvents(const QSet<QGesture *> &gestures,
                       QSet<QGesture *> *undeliveredGestures);
    void getGestureTargets(const QSet<QGesture*> &gestures,
                           QMap<QWidget *, QList<QGesture *> > *conflicts,
                           QMap<QWidget *, QList<QGesture *> > *normal);
};

QT_END_NAMESPACE

#endif // QGESTUREMANAGER_P_H
