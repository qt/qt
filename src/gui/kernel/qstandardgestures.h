/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSTANDARDGESTURES_H
#define QSTANDARDGESTURES_H

#include <QtGui/qevent.h>
#include <QtCore/qbasictimer.h>

#include <QtGui/qgesture.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QPanGesturePrivate;
class Q_GUI_EXPORT QPanGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPanGesture)

    Q_PROPERTY(QSize totalOffset READ totalOffset)
    Q_PROPERTY(QSize lastOffset READ lastOffset)

public:
    QPanGesture(QWidget *parent);

    bool filterEvent(QEvent *event);

    QSize totalOffset() const;
    QSize lastOffset() const;

protected:
    void reset();

private:
    bool event(QEvent *event);
    bool eventFilter(QObject *receiver, QEvent *event);

    friend class QWidget;
};

class QPinchGesturePrivate;
class Q_GUI_EXPORT QPinchGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPinchGesture)

    Q_PROPERTY(qreal scaleFactor READ scaleFactor)
    Q_PROPERTY(qreal lastScaleFactor READ lastScaleFactor)

    Q_PROPERTY(qreal rotationAngle READ rotationAngle)
    Q_PROPERTY(qreal lastRotationAngle READ lastRotationAngle)

    Q_PROPERTY(QPoint startCenterPoint READ startCenterPoint)
    Q_PROPERTY(QPoint lastCenterPoint READ lastCenterPoint)
    Q_PROPERTY(QPoint centerPoint READ centerPoint)

public:
    QPinchGesture(QWidget *parent);

    bool filterEvent(QEvent *event);
    void reset();

    QPoint startCenterPoint() const;
    QPoint lastCenterPoint() const;
    QPoint centerPoint() const;

    qreal scaleFactor() const;
    qreal lastScaleFactor() const;

    qreal rotationAngle() const;
    qreal lastRotationAngle() const;

private:
    bool event(QEvent *event);
    bool eventFilter(QObject *receiver, QEvent *event);

    friend class QWidget;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSTANDARDGESTURES_H
