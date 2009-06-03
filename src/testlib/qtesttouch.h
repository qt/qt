/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTTOUCH_H
#define QTESTTOUCH_H

#if 0
// inform syncqt
#pragma qt_no_master_include
#endif

#include <QtTest/qtest_global.h>
#include <QtTest/qtestassert.h>
#include <QtTest/qtestsystem.h>
#include <QtTest/qtestspontaneevent.h>

#include <QtCore/qmap.h>
#include <QtGui/qevent.h>
#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

namespace QTest
{

    class QTouchEventSequence
    {
    public:
        ~QTouchEventSequence()
        {
            commit();
            foreach(QTouchEvent::TouchPoint *pt, points)
                delete pt;
            points.clear();
        }
        QTouchEventSequence& press(int touchId, const QPoint &pt)
        {
            touchPointStates |= Qt::TouchPointPressed;
            QTouchEvent::TouchPoint *p = point(touchId);
            p->setStartPos(pt);
            p->setStartScreenPos(widget->mapToGlobal(pt));
            p->setLastPos(pt);
            p->setLastScreenPos(widget->mapToGlobal(pt));
            p->setPos(pt);
            p->setScreenPos(widget->mapToGlobal(pt));
            p->setState(Qt::TouchPointPressed);
            return *this;
        }
        QTouchEventSequence& move(int touchId, const QPoint &pt)
        {
            touchPointStates |= Qt::TouchPointMoved;
            QTouchEvent::TouchPoint *p = point(touchId);
            p->setLastPos(p->pos());
            p->setLastScreenPos(widget->mapToGlobal(p->pos().toPoint()));
            p->setPos(pt);
            p->setScreenPos(widget->mapToGlobal(pt));
            p->setState(Qt::TouchPointMoved);
            return *this;
        }
        QTouchEventSequence& release(int touchId, const QPoint &pt)
        {
            touchPointStates |= Qt::TouchPointReleased;
            QTouchEvent::TouchPoint *p = point(touchId);
            p->setLastPos(p->pos());
            p->setLastScreenPos(widget->mapToGlobal(p->pos().toPoint()));
            p->setPos(pt);
            p->setScreenPos(widget->mapToGlobal(pt));
            p->setState(Qt::TouchPointReleased);
            return *this;
        }
        QTouchEventSequence& stationary(int touchId)
        {
            touchPointStates |= Qt::TouchPointStationary;
            QTouchEvent::TouchPoint *p = point(touchId);
            p->setState(Qt::TouchPointStationary);
            return *this;
        }

    private:
        QTouchEventSequence(QWidget *widget)
            : widget(widget)
        {
        }
        QTouchEventSequence(const QTouchEventSequence &v);
        void operator=(const QTouchEventSequence&);

        QTouchEvent::TouchPoint* point(int touchId)
        {
            QTouchEvent::TouchPoint *pt = points.value(touchId, 0);
            if (!pt) {
                pt = new QTouchEvent::TouchPoint;
                pt->setId(touchId);
                points.insert(touchId, pt);
            }
            return pt;
        }
        void commit()
        {
            if (widget) {
                QTouchEvent event(QEvent::RawTouch, Qt::NoModifier,
                                  touchPointStates, points.values());
                QSpontaneKeyEvent::setSpontaneous(&event);
                if (!qApp->notify(widget, &event))
                    QTest::qWarn("Touch event not accepted by receiving widget");
                widget = 0;
            }
        }

        QMap<int, QTouchEvent::TouchPoint*> points;
        QWidget *widget;
        Qt::TouchPointStates touchPointStates;
        friend QTouchEventSequence touchEvent(QWidget*);
    };

    QTouchEventSequence touchEvent(QWidget *widget)
    {
        return QTouchEventSequence(widget);
    }

}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QTESTTOUCH_H
