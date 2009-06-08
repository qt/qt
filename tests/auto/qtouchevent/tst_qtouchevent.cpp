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

#include <QtGui>
#include <QtTest>

class tst_QTouchEvent : public QObject
{
    Q_OBJECT
public:
    tst_QTouchEvent() { }
    ~tst_QTouchEvent() { }

private slots:
    void touchDisabledByDefault();
    void touchEventAcceptedByDefault();
};

void tst_QTouchEvent::touchDisabledByDefault()
{
    // the widget attribute is not enabled by default
    QWidget widget;
    QVERIFY(!widget.testAttribute(Qt::WA_AcceptTouchEvents));

    // events should not be accepted since they are not enabled
    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(QTouchEvent::TouchPoint(0));
    QTouchEvent touchEvent(QEvent::TouchBegin,
                           Qt::NoModifier,
                           Qt::TouchPointPressed,
                           touchPoints);
    bool res = QApplication::sendEvent(&widget, &touchEvent);
    QVERIFY(!res);
    QVERIFY(!touchEvent.isAccepted());
}

void tst_QTouchEvent::touchEventAcceptedByDefault()
{
    // enabling touch events should automatically accept touch events
    QWidget widget;
    widget.setAttribute(Qt::WA_AcceptTouchEvents);

    QList<QTouchEvent::TouchPoint> touchPoints;
    touchPoints.append(QTouchEvent::TouchPoint(0));
    QTouchEvent touchEvent(QEvent::TouchBegin,
                           Qt::NoModifier,
                           Qt::TouchPointPressed,
                           touchPoints);
    bool res = QApplication::sendEvent(&widget, &touchEvent);
    QVERIFY(!res); // not handled...
    QVERIFY(touchEvent.isAccepted()); // but accepted
}

QTEST_MAIN(tst_QTouchEvent)

#include "tst_qtouchevent.moc"
