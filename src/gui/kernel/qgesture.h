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

#ifndef QGESTURE_H
#define QGESTURE_H

#include "qobject.h"
#include "qlist.h"
#include "qdatetime.h"
#include "qpoint.h"
#include "qrect.h"
#include "qmetatype.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class QGesturePrivate;
class Q_GUI_EXPORT QGesture : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGesture)

    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(Qt::GestureState state READ state)

    Q_PROPERTY(QDateTime startTime READ startTime)
    Q_PROPERTY(uint duration READ duration)

    Q_PROPERTY(QRect rect READ rect WRITE setRect)
    Q_PROPERTY(QPoint hotSpot READ hotSpot WRITE setHotSpot)
    Q_PROPERTY(QPoint startPos READ startPos WRITE setStartPos)
    Q_PROPERTY(QPoint lastPos READ lastPos WRITE setLastPos)
    Q_PROPERTY(QPoint pos READ pos WRITE setPos)

public:
    QGesture(QObject *parent, const QString &type,
             Qt::GestureState state = Qt::GestureStarted);
    QGesture(QObject *parent,
             const QString &type, const QPoint &startPos,
             const QPoint &lastPos, const QPoint &pos, const QRect &rect,
             const QPoint &hotSpot, const QDateTime &startTime,
             uint duration, Qt::GestureState state);
    virtual ~QGesture();

    inline void setAccepted(bool accepted) { m_accept = accepted; }
    inline bool isAccepted() const { return m_accept; }

    inline void accept() { m_accept = true; }
    inline void ignore() { m_accept = false; }

    QString type() const;
    Qt::GestureState state() const;

    QDateTime startTime() const;
    uint duration() const;

    QRect rect() const;
    void setRect(const QRect &rect);
    QPoint hotSpot() const;
    void setHotSpot(const QPoint &point);

    QPoint startPos() const;
    void setStartPos(const QPoint &point);
    QPoint lastPos() const;
    void setLastPos(const QPoint &point);
    QPoint pos() const;
    void setPos(const QPoint &point);

protected:
    QGesture(QGesturePrivate &dd, QObject *parent, const QString &type,
             Qt::GestureState state = Qt::GestureStarted);
    virtual void translate(const QPoint &offset);

private:
    ushort m_accept : 1;

    friend class QGestureManager;
    friend class QApplication;
    friend class QGraphicsScene;
    friend class QGraphicsScenePrivate;
    friend class QGestureRecognizerPan;
    friend class QDoubleTapGestureRecognizer;
    friend class QTapAndHoldGestureRecognizer;
};

class QPanningGesturePrivate;
class Q_GUI_EXPORT QPanningGesture : public QGesture
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QPanningGesture)

    Q_PROPERTY(Qt::DirectionType lastDirection READ lastDirection)
    Q_PROPERTY(Qt::DirectionType direction READ direction)

public:
    Qt::DirectionType lastDirection() const;
    Qt::DirectionType direction() const;

private:
    QPanningGesture(QObject *parent = 0);
    ~QPanningGesture();

    friend class QGestureRecognizerPan;
};

Q_DECLARE_METATYPE(Qt::DirectionType)
Q_DECLARE_METATYPE(Qt::GestureState)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGESTURE_H
