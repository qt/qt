/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef INPUTCONTEXTFILTER_H
#define INPUTCONTEXTFILTER_H

#include <QtDeclarative/qdeclarative.h>
#include <QtGui/qevent.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class InputContextKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    InputContextKeyEvent(const QKeyEvent &ke)
        : event(ke) { event.setAccepted(false); }

    int key() const { return event.key(); }
    QString text() const { return event.text(); }
    int modifiers() const { return event.modifiers(); }
    bool isAutoRepeat() const { return event.isAutoRepeat(); }
    int count() const { return event.count(); }

    bool isAccepted() { return event.isAccepted(); }
    void setAccepted(bool accepted) { event.setAccepted(accepted); }

private:
    QKeyEvent event;
};

class InputContextMouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    InputContextMouseEvent(const QMouseEvent &me)
        : event(me) { event.setAccepted(false);}

    int x() const { return event.x(); }
    int y() const { return event.y(); }
    int button() const { return event.button(); }
    int buttons() const { return event.buttons(); }
    int modifiers() const { return event.modifiers(); }

    bool isAccepted() { return event.isAccepted(); }
    void setAccepted(bool accepted) { event.setAccepted(accepted); }

private:
    QMouseEvent event;
};

class InputContextMouseHandler : public QObject
{
    Q_OBJECT
public:
    InputContextMouseHandler(QObject *parent = 0);

    void processEvent(QEvent::Type type, int cursor, InputContextMouseEvent *event);

signals:
    void pressed(int cursor, InputContextMouseEvent *mouse);
    void released(int cursor, InputContextMouseEvent *mouse);
    void doubleClicked(int cursor, InputContextMouseEvent *mouse);
    void positionChanged(int cursor, InputContextMouseEvent *mouse);
};

class InputContextMouseFilter : public QObject
{
    Q_OBJECT
public:
    InputContextMouseFilter(QObject *parent = 0);

    void processEvent(QEvent::Type type, InputContextMouseEvent *event);

signals:
    void pressed(InputContextMouseEvent *mouse);
    void released(InputContextMouseEvent *mouse);
    void doubleClicked(InputContextMouseEvent *mouse);
    void positionChanged(InputContextMouseEvent *mouse);
};

class InputContextKeyFilter : public QObject
{
    Q_OBJECT
public:
    InputContextKeyFilter(QObject *parent = 0);

    void processEvent(QEvent::Type type, InputContextKeyEvent *event);

signals:
    void pressed(InputContextKeyEvent *event);
    void released(InputContextKeyEvent *event);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(InputContextKeyEvent)
QML_DECLARE_TYPE(InputContextMouseEvent)
QML_DECLARE_TYPE(InputContextMouseHandler)
QML_DECLARE_TYPE(InputContextMouseFilter)
QML_DECLARE_TYPE(InputContextKeyFilter)

QT_END_HEADER

#endif
